#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include "LegalMate.h"

// main.cpp - by Ali Sajjad
// console menu for legalmate
// the menu only talks to the LegalMate class, never to the structures directly

// reads one full line, ends the program if input is closed
std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string line;
    if (!std::getline(std::cin, line)) {
        std::cout << std::endl;
        std::exit(0);
    }
    return line;
}

// keeps asking until the user types a whole number between min and max
int readNumber(const std::string& prompt, int min, int max) {
    while (true) {
        std::string line = readLine(prompt);

        // only digits allowed, and max 6 of them so stoi cant overflow
        bool valid = !line.empty() && line.length() <= 6;
        for (size_t i = 0; i < line.length(); i++) {
            if (line[i] < '0' || line[i] > '9') {
                valid = false;
            }
        }

        if (valid) {
            int number = std::stoi(line);
            if (number >= min && number <= max) {
                return number;
            }
        }
        std::cout << "Please enter a number from " << min << " to " << max << "." << std::endl;
    }
}

// 7 -> "DOC-007"
std::string makeIdString(int id) {
    std::string number = std::to_string(id);
    while (number.length() < 3) {
        number = "0" + number;
    }
    return "DOC-" + number;
}

// prints a full document
void printDocument(const Document& doc) {
    std::cout << std::endl;
    std::cout << doc.id_str << "  " << doc.title << std::endl;
    std::cout << "Category: " << doc.category << std::endl;
    std::cout << "Tags: ";
    for (size_t i = 0; i < doc.tags.size(); i++) {
        std::cout << doc.tags[i];
        if (i + 1 < doc.tags.size()) {
            std::cout << ", ";
        }
    }
    std::cout << std::endl << std::endl << doc.text << std::endl;
}

// shows up to 5 related docs from the graph
void printRecommendations(LegalMate& app, int id) {
    LinkedList<Recommendation> recs = app.recommend(id);
    if (recs.isEmpty()) {
        std::cout << std::endl << "No related documents." << std::endl;
        return;
    }

    std::cout << std::endl << "You may also need:" << std::endl;
    ListNode<Recommendation>* current = recs.getHead();
    int shown = 0;

    while (current != nullptr && shown < 5) {
        Document* doc = app.findById(current->data.docId);
        std::cout << "  " << doc->id_str << "  " << doc->title;
        if (current->data.level == 1) {
            std::cout << "  (" << current->data.weight << " shared tags)";
        } else {
            std::cout << "  (via " << app.findById(current->data.viaDocId)->id_str << ")";
        }
        std::cout << std::endl;
        shown++;
        current = current->next;
    }
}

// option 2, also used after search
void viewDocument(LegalMate& app, int id) {
    Document* doc = app.findById(id);
    if (doc == nullptr) {
        std::cout << "No document with ID " << makeIdString(id) << "." << std::endl;
        return;
    }
    printDocument(*doc);
    printRecommendations(app, id);
}

// option 1
void searchMenu(LegalMate& app) {
    std::string query = readLine("Enter keywords: ");
    LinkedList<SearchResult> results = app.search(query);

    if (results.isEmpty()) {
        std::cout << "No documents found." << std::endl;
        return;
    }

    std::cout << std::endl << results.size() << " documents found:" << std::endl;
    int rank = 1;
    ListNode<SearchResult>* current = results.getHead();
    while (current != nullptr) {
        Document* doc = app.findById(current->data.docId);
        std::cout << "  " << rank << ". " << doc->id_str << "  " << doc->title
                  << "  (score " << current->data.score << ")" << std::endl;
        rank++;
        current = current->next;
    }

    // let the user open one result straight away
    int id = readNumber("\nEnter a document number to open (e.g. 3 for DOC-003), or 0 to go back: ", 0, 999);
    if (id != 0) {
        viewDocument(app, id);
    }
}

// option 3
void browseMenu(LegalMate& app) {
    LinkedList<std::string> names = app.getCategoryNames();
    if (names.isEmpty()) {
        std::cout << "No categories yet." << std::endl;
        return;
    }

    // show categories as numbered options
    std::cout << std::endl;
    int number = 1;
    ListNode<std::string>* current = names.getHead();
    while (current != nullptr) {
        std::cout << "  " << number << ". " << current->data << std::endl;
        number++;
        current = current->next;
    }

    int choice = readNumber("Choose a category (0 to go back): ", 0, names.size());
    if (choice == 0) {
        return;
    }

    // walk to the chosen name
    current = names.getHead();
    for (int i = 1; i < choice; i++) {
        current = current->next;
    }

    std::cout << std::endl << current->data << ":" << std::endl;
    const LinkedList<int>* ids = app.getCategory(current->data);
    ListNode<int>* idNode = ids->getHead();
    while (idNode != nullptr) {
        Document* doc = app.findById(idNode->data);
        std::cout << "  " << doc->id_str << "  " << doc->title << std::endl;
        idNode = idNode->next;
    }
}

// splits "tenant;rent;notice" into separate tags, spaces are ignored
std::vector<std::string> splitTags(const std::string& line) {
    std::vector<std::string> tags;
    std::string tag = "";

    for (size_t i = 0; i < line.length(); i++) {
        if (line[i] == ';') {
            // end of one tag, keep it if its not empty
            if (tag != "") {
                tags.push_back(tag);
            }
            tag = "";
        } else if (line[i] != ' ') {
            tag += line[i];
        }
    }

    // the last tag has no ; after it
    if (tag != "") {
        tags.push_back(tag);
    }
    return tags;
}

// option 5, returns true if a doc was added
bool addMenu(LegalMate& app) {
    int id = readNumber("Document number (e.g. 31 for DOC-031): ", 1, 999);
    if (app.findById(id) != nullptr) {
        std::cout << makeIdString(id) << " already exists." << std::endl;
        return false;
    }

    std::string title = readLine("Title: ");
    std::string category = readLine("Category: ");
    std::string tagLine = readLine("Tags separated by ; (e.g. rent;notice): ");
    std::string text = readLine("Text: ");

    // | splits the fields in the csv file so it cant be inside any field
    std::string all = title + category + tagLine + text;
    if (title.empty() || category.empty() || text.empty() || all.find('|') != std::string::npos) {
        std::cout << "Title, category and text are required, and | is not allowed." << std::endl;
        return false;
    }

    Document doc(id, makeIdString(id), title, category, splitTags(tagLine), text);
    app.addDocument(doc);
    std::cout << doc.id_str << " added." << std::endl;
    return true;
}

// option 6, returns true if a doc was deleted
bool deleteMenu(LegalMate& app) {
    int id = readNumber("Document number to delete: ", 1, 999);
    Document* doc = app.findById(id);
    if (doc == nullptr) {
        std::cout << "No document with ID " << makeIdString(id) << "." << std::endl;
        return false;
    }

    // ask first, deleting cant be undone
    std::string answer = readLine("Delete " + doc->id_str + " " + doc->title + "? (y/n): ");
    if (answer != "y" && answer != "Y") {
        std::cout << "Cancelled." << std::endl;
        return false;
    }

    app.deleteDocument(id);
    std::cout << "Deleted." << std::endl;
    return true;
}

int main() {
    LegalMate app("documents.csv");
    app.loadData();
    bool unsaved = false;   // true after add or delete, so we know to save on exit

    while (true) {
        std::cout << std::endl << "LegalMate - Legal Document Explorer" << std::endl;
        std::cout << app.documentCount() << " documents | " << app.keywordCount()
                  << " keywords | " << app.linkCount() << " links" << std::endl;
        std::cout << "  1. Search by keyword" << std::endl;
        std::cout << "  2. View a document" << std::endl;
        std::cout << "  3. Browse by category" << std::endl;
        std::cout << "  4. List all titles (A-Z)" << std::endl;
        std::cout << "  5. Add a document" << std::endl;
        std::cout << "  6. Delete a document" << std::endl;
        std::cout << "  7. Save" << std::endl;
        std::cout << "  0. Save and exit" << std::endl;

        int choice = readNumber("Choice: ", 0, 7);

        if (choice == 1) {
            searchMenu(app);
        } else if (choice == 2) {
            viewDocument(app, readNumber("Document number (e.g. 3 for DOC-003): ", 1, 999));
        } else if (choice == 3) {
            browseMenu(app);
        } else if (choice == 4) {
            std::cout << std::endl;
            app.listAllTitles();
        } else if (choice == 5) {
            if (addMenu(app)) {
                unsaved = true;
            }
        } else if (choice == 6) {
            if (deleteMenu(app)) {
                unsaved = true;
            }
        } else if (choice == 7) {
            app.saveData();
            unsaved = false;
        } else {
            // choice 0, save only if something changed
            if (unsaved) {
                app.saveData();
            }
            std::cout << "Goodbye." << std::endl;
            break;
        }
    }
    return 0;
}