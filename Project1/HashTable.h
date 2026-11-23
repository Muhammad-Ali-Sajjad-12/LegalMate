#pragma once
#include <string>
#include "LinkedList.h"

// hashtable.h - by sajjad
// keyword index, maps each keyword to the doc ids that have it like "rent" -> 3, 7, 12
// collisions handled by chaining, each bucket is a linked list
// average lookup is O(1)

// One keyword and the IDs of the documents that contain it
struct WordEntry {
    std::string word;
    LinkedList<int> docIds;

    // Entries are matched by word only, so a bucket can be searched with find()
    bool operator==(const WordEntry& other) const {
        return word == other.word;
    }
};

class HashTable {
private:
    static const int tableSize = 211;          // number of buckets (a prime number)
    LinkedList<WordEntry> buckets[tableSize];  // each bucket is a chain of entries
    int wordCount;                             // number of different keywords stored

public:
    HashTable() : wordCount(0) {}

    // Turns a word into a bucket number from 0 to tableSize - 1
    int hashFunction(const std::string& word) const {
        int hash = 0;

        for (size_t i = 0; i < word.length(); i++) {
            hash = (hash * 31 + word[i]) % tableSize;
        }
        return hash;
    }

    // Records that document docId contains word
    void insertWord(const std::string& word, int docId) {
        int index = hashFunction(word);

        // A temporary entry holding only the word, used to search the bucket
        WordEntry key;
        key.word = word;
        WordEntry* entry = buckets[index].find(key);

        if (entry == nullptr) {
            // First time we see this word: create its entry in the bucket
            key.docIds.pushBack(docId);
            buckets[index].pushBack(key);
            wordCount++;
        } else if (!entry->docIds.contains(docId)) {
            // Word already indexed: add this document to its list
            entry->docIds.pushBack(docId);
        }
    }

    int getWordCount() const {
        return wordCount;
    }

    // Returns the IDs of the documents that contain word,
    // or nullptr if the word is not in the index
    const LinkedList<int>* searchWord(const std::string& word) {
        int index = hashFunction(word);

        WordEntry key;
        key.word = word;
        WordEntry* entry = buckets[index].find(key);

        if (entry == nullptr) {
            return nullptr;
        }
        return &entry->docIds;
    }

    // Removes docId from every keyword. Used when a document is deleted.
    // A keyword left with no documents is removed completely.
    void removeDocument(int docId) {
        for (int i = 0; i < tableSize; i++) {
            ListNode<WordEntry>* current = buckets[i].getHead();

            while (current != nullptr) {
                // Save the next node first: current may be deleted below
                ListNode<WordEntry>* nextNode = current->next;

                current->data.docIds.remove(docId);

                if (current->data.docIds.isEmpty()) {
                    WordEntry key;
                    key.word = current->data.word;
                    buckets[i].remove(key);
                    wordCount--;
                }
                current = nextNode;
            }
        }
    }

    // Prints every keyword with its bucket number and document IDs
    void display() const {
        for (int i = 0; i < tableSize; i++) {
            ListNode<WordEntry>* current = buckets[i].getHead();

            while (current != nullptr) {
                std::cout << "[" << i << "] " << current->data.word << ": ";
                current->data.docIds.display();
                current = current->next;
            }
        }
    }
};