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
#include "Graph.h"
#include "TextUtils.h"

// legalmate.h - by sajjad
// main class that connects every data structure
// the menu only talks to this class, never to the structures directly
// when a doc is added or deleted this class updates all structures together
// so they never go out of sync
// avl tree is the main store, others keep ids or copies for their own job
// graph (ahmad) links docs that share tags used for recommendations

class LegalMate {
private:
    AVLTree documents;          // main store, find any doc by id
    BinarySearchTree titles;    // docs sorted by title for a-z list
    HashTable keywordIndex;     // keyword -> doc ids
    CategoryIndex categories;   // category -> doc ids
    Graph relations;            // links docs that share tags, for recommendations
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

        // copy the doc with every tag in lowercase so "Tenant" and "tenant" match
        Document clean = doc;
        for (size_t i = 0; i < clean.tags.size(); i++) {
            clean.tags[i] = toLowerCase(clean.tags[i]);
        }

        documents.insert(clean);                            // avl, main store
        titles.insert(clean);                               // bst, for a-z list
        indexDocument(clean, keywordIndex);                 // hash table, for search
        categories.addDocument(clean.category, clean.id);   // category list
        relations.addDocument(clean);                       // graph, for recommendations
        allIds.pushBack(clean.id);                          // for saving later
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
        relations.removeDocument(id);
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

    // returns all documents sorted A-Z by title (for Qt UI)
    std::vector<Document> getTitlesSorted() {
        return titles.getAllSorted();
    }

    int avlHeight() {
        return documents.treeHeight();
    }

    int documentCount() const {
        return allIds.size();
    }

    int keywordCount() const {
        return keywordIndex.getWordCount();
    }

    // related docs from the graph, direct neighbours first then 2 steps away
    LinkedList<Recommendation> recommend(int id) {
        return relations.recommend(id, 2);
    }

    int linkCount() const {
        return relations.edgeCount();
    }
};