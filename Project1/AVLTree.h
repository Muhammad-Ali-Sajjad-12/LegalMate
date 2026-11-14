#ifndef AVLTREE_H
#define AVLTREE_H
#include "Document.h"
#include <iostream>
#include <algorithm>

using namespace std;

class AVLNode
{
public:
    Document documnet;
    AVLNode* left;
    AVLNode* right;
    int height;

    AVLNode(Document d)
    {
        this->documnet = d;
        left = right = nullptr;
        height = 1;
    }
};

class AVLTree
{
private:
    AVLNode* root;

    int getHeight(AVLNode* n)
    {
        if (n == nullptr)
            return 0;
        return n->height;
    }

    int getBalanceFactor(AVLNode* n)
    {
        if (n == nullptr)
            return 0;
        return getHeight(n->left) - getHeight(n->right);
    }

    // Right rotation 
    AVLNode* right_rotate(AVLNode* y)
    {
        AVLNode* x = y->left;
        AVLNode* T2 = x->right;

        x->right = y;
        y->left = T2;

        y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
        x->height = max(getHeight(x->left), getHeight(x->right)) + 1;

        return x;
    }

    // Left rotation 
    AVLNode* left_rotate(AVLNode* x)
    {
        AVLNode* y = x->right;
        AVLNode* T2 = y->left;
        
        y->left = x;
        x->right = T2;

        x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
        y->height = max(getHeight(y->left), getHeight(y->right)) + 1;

        return y;
    }

    AVLNode* insert(AVLNode* node, Document d)
    {
        if (node == nullptr)
        {
            return new AVLNode(d);
        }

        if (d.id < node->documnet.id)
        {
            node->left = insert(node->left, d);
        }
        else if (d.id > node->documnet.id)
        {
            node->right = insert(node->right, d);
        }
        else
        {
            return node; // Duplicate ID not allowed
        }

        // Update height
        node->height = 1 + max(getHeight(node->left), getHeight(node->right));
        int bf = getBalanceFactor(node);

        // Case 1: Left Left (LL)
        if (bf > 1 && d.id < node->left->documnet.id)
        {
            return right_rotate(node);
        }

        // Case 2: Right Right (RR)
        if (bf < -1 && d.id > node->right->documnet.id)
        {
            return left_rotate(node);
        }

        // Case 3: Left Right (LR)
        if (bf > 1 && d.id > node->left->documnet.id)
        {
            node->left = left_rotate(node->left);
            return right_rotate(node);
        }

        // Case 4: Right Left (RL)
        if (bf < -1 && d.id < node->right->documnet.id)
        {
            node->right = right_rotate(node->right);
            return left_rotate(node);
        }

        return node;
    }

    AVLNode* in_order_successor(AVLNode* node)
    {
        AVLNode* current = node;
        while (current->left != nullptr)
            current = current->left;
        return current;
    }

    AVLNode* remove(AVLNode* root, int id)
    {
        if (root == nullptr)
            return root;

        if (id < root->documnet.id)
            root->left = remove(root->left, id);
        else if (id > root->documnet.id)
            root->right = remove(root->right, id);
        else
        {
            // Node with only one child or no child
            if ((root->left == nullptr) || (root->right == nullptr))
            {
                AVLNode* temp = root->left ? root->left : root->right;
                if (temp == nullptr)
                {
                    temp = root;
                    root = nullptr;
                }
                else
                {
                    *root = *temp;
                }
                delete temp;
            }
            else
            {
                // Node with two children: get in-order successor
                AVLNode* temp = in_order_successor(root->right);
                root->documnet = temp->documnet;
                root->right = remove(root->right, temp->documnet.id);
            }
        }

        if (root == nullptr)
            return root;

        // Update height
        root->height = 1 + max(getHeight(root->left), getHeight(root->right));
        int bf = getBalanceFactor(root);

        // Rebalance after deletion
        if (bf > 1 && getBalanceFactor(root->left) >= 0)
            return right_rotate(root);

        if (bf > 1 && getBalanceFactor(root->left) < 0)
        {
            root->left = left_rotate(root->left);
            return right_rotate(root);
        }

        if (bf < -1 && getBalanceFactor(root->right) <= 0)
            return left_rotate(root);

        if (bf < -1 && getBalanceFactor(root->right) > 0)
        {
            root->right = right_rotate(root->right);
            return left_rotate(root);
        }

        return root;
    }

    Document* search(AVLNode* node, int id)
    {
        if (node == nullptr)
            return nullptr;

        if (id == node->documnet.id)
            return &(node->documnet);
        else if (id < node->documnet.id)
            return search(node->left, id);
        else
            return search(node->right, id);
    }

    void inorder(AVLNode* root)
    {
        if (root == nullptr)
            return;

        inorder(root->left);
        cout << "ID: " << root->documnet.id << " (" << root->documnet.id_str << ") | Title: " << root->documnet.title << endl;
        inorder(root->right);
    }

public:
    AVLTree()
    {
        root = nullptr;
    }

    void insert(Document d)
    {
        root = insert(root, d);
    }

    void remove(int id)
    {
        root = remove(root, id);
    }

	Document* search(int id)         // search (tree.root, id) instead of search(id) to search in the tree
    {
        return search(root, id);
    }

    void inorder()
    {
        inorder(root);
    }

    AVLNode* getRoot()
    {
        return root;
    }
};

#endif