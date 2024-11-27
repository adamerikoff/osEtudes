// main.c
#include <stdio.h>
#include "bst.h"

void log(const char* message) {
    printf("[LOG]: %s\n", message);
}

int main() {
    Node* root = NULL;

    // Insert nodes
    log("Inserting nodes into the BST...");
    root = insert(root, 50);
    root = insert(root, 30);
    root = insert(root, 70);
    root = insert(root, 20);
    root = insert(root, 40);
    root = insert(root, 60);
    root = insert(root, 80);

    // Print the BST (in-order traversal)
    log("In-order traversal of the BST:");
    inorder_traversal(root);
    printf("\n");

    // Search for a node
    int key = 30;
    log("Searching for a node...");
    Node* found_node = search(root, key);
    if (found_node != NULL) {
        printf("Node with key %d found.\n", key);
    } else {
        printf("Node with key %d not found.\n", key);
    }

    // Find the minimum and maximum values
    log("Finding the minimum and maximum values...");
    Node* min_node = find_min(root);
    Node* max_node = find_max(root);
    printf("Minimum value in the BST: %d\n", min_node->data);
    printf("Maximum value in the BST: %d\n", max_node->data);

    // Delete a node
    log("Deleting a node with key 20...");
    root = delete_node(root, 20);
    log("In-order traversal after deletion:");
    inorder_traversal(root);
    printf("\n");

    // Free the memory allocated for the BST
    log("Freeing the memory allocated for the BST...");
    free_tree(root);

    return 0;
}
