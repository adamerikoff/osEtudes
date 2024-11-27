// bst.h
#ifndef BST_H
#define BST_H

typedef struct Node {
    int data;
    struct Node* left;
    struct Node* right;
} Node;

Node* create_node(int data);
Node* insert(Node* root, int data);
Node* search(Node* root, int key);
Node* find_min(Node* root);
Node* find_max(Node* root);
Node* delete_node(Node* root, int key);
void inorder_traversal(Node* root);
void free_tree(Node* root);

#endif
