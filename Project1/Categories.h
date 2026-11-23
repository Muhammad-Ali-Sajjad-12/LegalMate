#pragma once
#include <iostream>
#include <string>
#include "LinkedList.h"

// categories.h - by sajjad
// groups documents by category like "Property & Tenancy" -> 3, 7, 12
// its a linked list of categories and each one has its own linked list of doc ids
// only ids are stored here, full docs come from the avl tree

// one category with its doc ids
struct CategoryList {
    std::string name;
    LinkedList<int> docIds;

    // compare by name only so find() works with just the name
    bool operator==(const CategoryList& other) const {
        return name == other.name;
    }
};

class CategoryIndex {
private:
    LinkedList<CategoryList> categories;

public:
    // add doc id to its category, makes the category if its new
    void addDocument(const std::string& category, int docId) {
        CategoryList key;
        key.name = category;
        CategoryList* existing = categories.find(key);

        if (existing == nullptr) {
            key.docIds.pushBack(docId);
            categories.pushBack(key);
        } else if (!existing->docIds.contains(docId)) {
            existing->docIds.pushBack(docId);
        }
    }

    // returns doc ids of a category or nullptr if not found
    const LinkedList<int>* getCategory(const std::string& category) {
        CategoryList key;
        key.name = category;
        CategoryList* existing = categories.find(key);

        if (existing == nullptr) {
            return nullptr;
        }
        return &existing->docIds;
    }

    // remove a doc id from its category
    // if the category becomes empty we remove the whole category
    bool removeDocument(const std::string& category, int docId) {
        CategoryList key;
        key.name = category;
        CategoryList* existing = categories.find(key);

        if (existing == nullptr || !existing->docIds.remove(docId)) {
            return false;
        }

        if (existing->docIds.isEmpty()) {
            categories.remove(key);
        }
        return true;
    }

    // gives all category names, the menu will show these as options
    LinkedList<std::string> getCategoryNames() const {
        LinkedList<std::string> names;
        ListNode<CategoryList>* current = categories.getHead();

        while (current != nullptr) {
            names.pushBack(current->data.name);
            current = current->next;
        }
        return names;
    }

    // prints every category with how many docs it has
    void display() const {
        ListNode<CategoryList>* current = categories.getHead();

        while (current != nullptr) {
            std::cout << current->data.name << " (" << current->data.docIds.size() << "): ";
            current->data.docIds.display();
            current = current->next;
        }
    }
};