//DOCUMENTATION
// LinkedList.h
// Author: Muhammad Ali Sajjad
//
// Generic singly linked list used across LegalMate for document IDs
// (hash table), keywords (tokenizer) and edges (graph).
//
// - Template: one class works for any type, so all code stays in this header.
// - Keeps head and tail, so pushBack and pushFront are O(1).
//   Search and remove are O(n).
// - The list owns its nodes: they are created with new and freed in clear()
//   and the destructor.
// - Uses std:: instead of "using namespace std;" because a header is copied
//   into every file that includes it, which can cause name clashes
//   (e.g. our find/remove/count vs std::find/std::remove/std::count)
//   std has built-in functions as remove, count, etc

#pragma once
#include <iostream>

template <typename T> 
struct ListNode {
    T data;
    ListNode<T>* next;

    ListNode(const T& value) : data(value), next(nullptr) {} // Struct - Constructor
};

template <typename T>
class LinkedList {
private:
    ListNode<T>* head; //1st Node  
    ListNode<T>* tail;   // last node kept so adding at the end is O(1)
    int count;           // number of nodes

public:
    LinkedList() : head(nullptr), tail(nullptr), count(0) {}

    // Runs automatically when the list goes out of scope
    ~LinkedList() {
        clear();
    }

    // Copy Constructor (Deep Copy - prevents double-free crashes)
    LinkedList(const LinkedList<T>& other) : head(nullptr), tail(nullptr), count(0) {
        ListNode<T>* current = other.head;
        while (current != nullptr) {
            pushBack(current->data);
            current = current->next;
        }
    }

    // Assignment Operator (Deep Copy)
    LinkedList<T>& operator=(const LinkedList<T>& other) {
        if (this != &other) {
            clear();
            ListNode<T>* current = other.head;
            while (current != nullptr) {
                pushBack(current->data);
                current = current->next;
            }
        }
        return *this;
    }

    // Adds a value at the end of the list
    void pushBack(const T& value) {
        ListNode<T>* newNode = new ListNode<T>(value);

        if (head == nullptr) {
            // Empty list: the new node is both the first and the last
            head = newNode;
            tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
        count++;
    }

    // Adds a value at the start of the list
    void pushFront(const T& value) {
        ListNode<T>* newNode = new ListNode<T>(value);
        newNode->next = head;
        head = newNode;

        if (tail == nullptr) {
            // The list was empty, so this node is also the last one
            tail = newNode;
        }
        count++;
    }

    int size() const {
        return count;
    }

    bool isEmpty() const {
        return count == 0;
    }

    // Gives other classes the first node so they can walk through the list
    ListNode<T>* getHead() const {
        return head;
    }

    // Prints the list as: 1 -> 3 -> 12
    void display() const {
        ListNode<T>* current = head;

        while (current != nullptr) {
            std::cout << current->data;
            if (current->next != nullptr) {
                std::cout << " -> ";
            }
            current = current->next;
        }
        std::cout << std::endl;
    }

    // Returns true if the value is somewhere in the list
    bool contains(const T& value) const {
        ListNode<T>* current = head;

        while (current != nullptr) {
            if (current->data == value) {
                return true;
            }
            current = current->next;
        }
        return false;
    }

    // Returns the address of the stored value so the caller can change it,
    // or nullptr if the value is not in the list
    T* find(const T& value) {
        ListNode<T>* current = head;

        while (current != nullptr) {
            if (current->data == value) {
                return &current->data;
            }
            current = current->next;
        }
        return nullptr;
    }

    // Removes the first node holding this value
    // Returns false if the value is not in the list
    bool remove(const T& value) {
        ListNode<T>* previous = nullptr;
        ListNode<T>* current = head;

        while (current != nullptr) {
            if (current->data == value) {
                // Unlink the node from the chain
                if (previous == nullptr) {
                    head = current->next;          // removing the first node
                } else {
                    previous->next = current->next;
                }

                // If it was the last node, tail moves back one step
                if (current == tail) {
                    tail = previous;
                }

                delete current;
                count--;
                return true;
            }
            previous = current;
            current = current->next;
        }
        return false;
    }

    // Deletes every node and resets the list to empty
    void clear() {
        ListNode<T>* current = head;

        while (current != nullptr) {
            ListNode<T>* nextNode = current->next;   // save the next node first
            delete current;
            current = nextNode;
        }
        head = nullptr;
        tail = nullptr;
        count = 0;
    }

    // the next functions go here
};