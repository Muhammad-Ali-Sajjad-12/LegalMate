#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "Document.h"
#include "AVLTree.h"
#include "BST.h"
#include "HashTable.h"
#include "Categories.h"
#include "Search.h"
#include "FileManager.h"
#include "LinkedList.h"

// legalmate.h - by sajjad
// main class that connects every data structure
// the menu only talks to this class, never to the structures directly
// when a doc is added or deleted this class updates all structures together
// so they never go out of sync
// avl tree is the main store, others keep ids or copies for their own job
// graph (ahmad) will be added here once its pushed

class LegalMate {
private:
    AVLTree documents;          // main store, find any doc by id
    BinarySearchTree titles;    // docs sorted by title for a-z list
    HashTable keywordIndex;     // keyword -> doc ids
    CategoryIndex categories;   // category -> doc ids
    LinkedList<int> allIds;     // every doc id in the order added, used for saving
    FileManager files;          // loads and saves the csv file
    std::string dataFile;       // path of the csv file

public:
    LegalMate(const std::string& fileName) : dataFile(fileName) {}

    // adds a doc to every structure
    // returns false if a doc with the same id already exists
    bool addDocument(const Document& doc) {
        if (documents.search(doc.id) != nullptr) {
            return false;
        }

        documents.insert(doc);                          // avl, main store
        titles.insert(doc);                             // bst, for a-z list
        indexDocument(doc, keywordIndex);               // hash table, for search
        categories.addDocument(doc.category, doc.id);   // category list
        allIds.pushBack(doc.id);                        // for saving later
        return true;
    }

        // reads the csv using ahmads filemanager and adds every doc
    // returns how many docs were actually added
    int loadData() {
        std::vector<Document> loaded = files.loadDocuments(dataFile);
        int added = 0;

        for (size_t i = 0; i < loaded.size(); i++) {
            if (addDocument(loaded[i])) {
                added++;
            }
        }
        return added;
    }

    // collects every doc from the avl tree and writes them back to the csv
    bool saveData() {
        // dont write an empty file over existing data
        if (allIds.isEmpty()) {
            std::cout << "Nothing to save, file left unchanged." << std::endl;
            return false;
        }

        std::vector<Document> toSave;
        ListNode<int>* current = allIds.getHead();

        while (current != nullptr) {
            Document* doc = documents.search(current->data);
            if (doc != nullptr) {
                toSave.push_back(*doc);
            }
            current = current->next;
        }
        return files.saveDocuments(toSave, dataFile);
    }

    // removes a doc from every structure
    // returns false if no doc has this id
    bool deleteDocument(int id) {
        Document* doc = documents.search(id);
        if (doc == nullptr) {
            return false;
        }

        // copy what we need first, doc pointer is not safe after avl remove
        std::string category = doc->category;
        std::string title = doc->title;

        keywordIndex.removeDocument(id);
        categories.removeDocument(category, id);
        titles.remove(title);
        documents.remove(id);   // avl last, we needed the doc above
        allIds.remove(id);
        return true;
    }

    // these are what the menu calls
    Document* findById(int id) {
        return documents.search(id);
    }

    LinkedList<SearchResult> search(const std::string& query) {
        return searchKeywords(query, keywordIndex, documents);
    }

    const LinkedList<int>* getCategory(const std::string& name) {
        return categories.getCategory(name);
    }

    LinkedList<std::string> getCategoryNames() const {
        return categories.getCategoryNames();
    }

    // bst prints titles a-z
    void listAllTitles() {
        titles.inorder();
    }

    int documentCount() const {
        return allIds.size();
    }

    int keywordCount() const {
        return keywordIndex.getWordCount();
    }

    // recommend function goes here once ahmads graph is pushed
};