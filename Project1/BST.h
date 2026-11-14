#pragma once
#ifndef BST_H
#define BST_H

#include "Document.h"

class BSTNode
{
public:
    BSTNode* left;
    BSTNode* right;
    Document d;

    BSTNode(Document d)
    {
        left = right = nullptr;
        this->d = d;
    }
};

class BinarySearchTree
{
private:
    BSTNode* root;

    BSTNode* insert(BSTNode* root, Document d)
    {
        if (root == nullptr)
            return new BSTNode(d);

        if (d.title < root->d.title)
        {
            root->left = insert(root->left, d);
        }
        else if (d.title > root->d.title)
        {
            root->right = insert(root->right, d);
        }
        else
        {
            return root; // Duplicate title not allowed
        }
        return root;
    }

    BSTNode* search(BSTNode* root, string title)
    {
        if (root == nullptr || root->d.title == title)
            return root;
        if (title < root->d.title)
            return search(root->left, title);
        return search(root->right, title);
    }

    void inorder(BSTNode* root)
    {
        if (root == nullptr)
            return;
        inorder(root->left);
        cout << "[" << root->d.id_str << "] " << root->d.title << " (" << root->d.category << ")" << endl;
        inorder(root->right);
    }

    BSTNode* inorder_successor(BSTNode* node)
    {
        BSTNode* current = node;
        while (current && current->left != nullptr)
            current = current->left;
        return current;
    }

    BSTNode* remove(BSTNode* root, string title)
    {
        if (root == nullptr)
            return root;

        if (title < root->d.title)
            root->left = remove(root->left, title);
        else if (title > root->d.title)
            root->right = remove(root->right, title);
        else
        {
            BSTNode* temp = nullptr;
            if (root->left == nullptr)
            {
                temp = root->right;
                delete root;
                return temp;
            }
            else if (root->right == nullptr)
            {
                temp = root->left;
                delete root;
                return temp;
            }

            temp = inorder_successor(root->right);
            root->d = temp->d;
            root->right = remove(root->right, temp->d.title);
        }
        return root;
    }

public:
    BinarySearchTree()
    {
        root = nullptr;
    }

    void insert(Document d)
    {
        root = insert(root, d);
    }

    void remove(string title)
    {
        root = remove(root, title);
    }

    Document* search(string title)
    {
        BSTNode* result = search(root, title);
        if (result != nullptr)
            return &(result->d);
        return nullptr;
    }

    void inorder()
    {
        inorder(root);
    }

    BSTNode* getRoot()
    {
        return root;
    }
};

#endif
