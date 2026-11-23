#pragma once
#include <string>
#include "Document.h"
#include "AVLTree.h"
#include "HashTable.h"
#include "LinkedList.h"
#include "TextUtils.h"

// search.h - by sajjad
// keyword search for legalmate
// indexDocument puts a docs words into the hash table
// searchKeywords finds matching docs and scores them
// +3 if keyword is one of the docs tags, +1 if its only in title or text
// results are sorted best first
// avl tree is the main store, results only keep doc ids

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

// Sorts results from highest to lowest score using bubble sort.
// Equal scores are ordered by document ID so the output is always the same.
inline void sortByScore(LinkedList<SearchResult>& results) {
    bool swapped = true;

    while (swapped) {
        swapped = false;
        ListNode<SearchResult>* current = results.getHead();

        while (current != nullptr && current->next != nullptr) {
            SearchResult& first = current->data;
            SearchResult& second = current->next->data;

            bool outOfOrder = (second.score > first.score) ||
                              (second.score == first.score && second.docId < first.docId);

            if (outOfOrder) {
                SearchResult temp = first;
                first = second;
                second = temp;
                swapped = true;
            }
            current = current->next;
        }
    }
}

// Finds documents matching the query and gives each a relevance score:
// +3 for every keyword that is one of its tags, +1 if it only appears in the text
inline LinkedList<SearchResult> searchKeywords(const std::string& query,HashTable& index,AVLTree& documents) {
    LinkedList<SearchResult> results;
    LinkedList<std::string> keywords = tokenize(query);

    ListNode<std::string>* word = keywords.getHead();
    while (word != nullptr) {
        const LinkedList<int>* ids = index.searchWord(word->data);

        if (ids != nullptr) {
            ListNode<int>* idNode = ids->getHead();

            while (idNode != nullptr) {
                Document* doc = documents.search(idNode->data);

                if (doc != nullptr) {
                    int points = doc->hasTag(word->data) ? 3 : 1;

                    SearchResult key;
                    key.docId = doc->id;
                    key.score = points;

                    SearchResult* existing = results.find(key);
                    if (existing == nullptr) {
                        results.pushBack(key);          // first match for this document
                    } else {
                        existing->score += points;      // add to its running score
                    }
                }
                idNode = idNode->next;
            }
        }
        word = word->next;
    }

    sortByScore(results);
    return results;
}