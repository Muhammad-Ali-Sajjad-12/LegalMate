#pragma once
#include <string>
#include "Document.h"
#include "AVLTree.h"
#include "HashTable.h"
#include "LinkedList.h"
#include "TextUtils.h"

// Search.h
// Author: Muhammad Ali Sajjad
//
// Keyword search for LegalMate.
// - indexDocument: adds a document's words to the hash table
// - searchKeywords: finds matching documents and scores them
//   (+3 if a keyword is one of the document's tags, +1 if it only
//   appears in the title or text), best match first.
// - The AVL tree is the main store: results keep only document IDs,
//   and the full document is fetched from the tree when needed.

// One search hit: which document, and how relevant it is
struct SearchResult {
    int docId;
    int score;

    // Results are matched by document, so each document appears only once
    bool operator==(const SearchResult& other) const {
        return docId == other.docId;
    }
};

// Adds every keyword of a document (title, text and tags) to the index
inline void indexDocument(const Document& doc, HashTable& index) {
    // Join everything into one string so a single tokenize call handles it
    std::string allText = doc.title + " " + doc.text;
    for (size_t i = 0; i < doc.tags.size(); i++) {
        allText += " " + doc.tags[i];
    }

    LinkedList<std::string> keywords = tokenize(allText);

    ListNode<std::string>* current = keywords.getHead();
    while (current != nullptr) {
        index.insertWord(current->data, doc.id);
        current = current->next;
    }
}

// the next functions go here