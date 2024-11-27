#include <stdio.h>
#include <stdlib.h> // For malloc
#include <string.h> // For memcpy and strdup
#include <stdarg.h> // For va_list

#include "list.h"

// Function to create a new node
ListNode* createNode(DataType type, void *value) {
    ListNode *newNode = (ListNode *)malloc(sizeof(ListNode));
    if (!newNode) {
        return NULL; // Handle memory allocation failure
    }
    newNode->type = type;
    // Allocate memory for the data based on type
    switch (type) {
        case TYPE_INT:
            newNode->data = malloc(sizeof(int));
            if (newNode->data) {
                memcpy(newNode->data, value, sizeof(int));
            }
            break;
        case TYPE_FLOAT:
            newNode->data = malloc(sizeof(float));
            if (newNode->data) {
                memcpy(newNode->data, value, sizeof(float));
            }
            break;
        case TYPE_STRING:
            newNode->data = strdup((char *)value); // Duplicate string
            break;
        default:
            free(newNode); // Free the new node if type is invalid
            return NULL;
    }
    newNode->tail = NULL; // Initialize the tail pointer
    return newNode;
}

ListNode* List_list(DataType firstType, void *firstValue, ...) {
    ListNode *head = createNode(firstType, firstValue); // Create the head node
    if (!head) {
        return NULL; // Memory allocation failed
    }
    
    ListNode *tail = head; // Set tail to point to the head initially
    va_list args;          // Variable argument list
    va_start(args, firstValue); // Initialize the argument list

    while (1) {
        DataType type = va_arg(args, DataType); // Get the data type
        if (type == 0) { // 0 is an indicator to stop (you can change this)
            break; // Stop if type is 0 (or a specific value that indicates the end)
        }
        void *value;
        switch (type) {
            case TYPE_INT:
                value = malloc(sizeof(int));
                *(int *)value = va_arg(args, int);
                break;
            case TYPE_FLOAT:
                value = malloc(sizeof(float));
                *(float *)value = (float)va_arg(args, double); // Promote to double
                break;
            case TYPE_STRING:
                value = strdup(va_arg(args, char *)); // Allocate and duplicate string
                break;
            default:
                va_end(args);
                List_free(head); // Free memory in case of invalid type
                return NULL; // Invalid type
        }

        ListNode *newNode = createNode(type, value);
        if (!newNode) {
            List_free(head); // Free previously allocated nodes
            va_end(args);
            return NULL; // Memory allocation failed
        }

        tail->tail = newNode; // Link the current tail to the new node
        tail = newNode;       // Move the tail pointer to the new node
    }
    va_end(args); // Clean up the argument list
    return head;  // Return the head of the linked list
}


void List_print(ListNode *list) {
    ListNode *current = list;
    while (current) {
        switch (current->type) {
            case TYPE_INT:
                printf("%d ", *(int *)current->data);
                break;
            case TYPE_FLOAT:
                printf("%.2f ", *(float *)current->data);
                break;
            case TYPE_STRING:
                printf("%s ", (char *)current->data);
                break;
            default:
                printf("Unknown type ");
                break; // Ignore unknown types
        }
        current = current->tail;
    }
    printf("\n");
}


ListNode* List_append(ListNode* head, ListNode* newNode) {
    // If the list is empty, the new node becomes the head
    if (!head) {
        return newNode; // Return the new node as the new head
    }
    // Find the last node in the list
    ListNode *current = head;
    while (current->tail != NULL) { // Traverse until the end of the list
        current = current->tail;     // Move to the next node
    }
    // Append the new node
    current->tail = newNode;         // Link the last node to the new node
    newNode->tail = NULL;            // Ensure the new node's tail is NULL
    return head; // Return the head of the list (unchanged)
}

ListNode* List_copy(ListNode* list) {
    if (!list) {
        return NULL; // If the list is empty, return NULL
    }
    
    // Create a new node for the head of the new list
    ListNode* newHead = createNode(list->type, list->data); // Copy the head node
    if (!newHead) {
        return NULL; // Return NULL if memory allocation fails
    }

    ListNode* currentOriginal = list->tail; // Start from the second node in the original list
    ListNode* currentNew = newHead;          // Pointer to the current node in the new list
    
    while (currentOriginal) {
        ListNode* newNode = createNode(currentOriginal->type, currentOriginal->data);
        if (!newNode) {
            List_free(newHead); // Free previously allocated nodes if allocation fails
            return NULL; // Return NULL if allocation fails
        }
        currentNew->tail = newNode; // Link the new node to the new list
        currentNew = newNode;        // Move to the new node
        currentOriginal = currentOriginal->tail; // Move to the next node in the original list
    }

    return newHead; // Return the head of the copied list
}



ListNode* List_get(ListNode* list, int index) {
    if (index < 0) {
        return NULL; // Return NULL for invalid indices
    }
    ListNode *current = list; // Start from the head of the list
    int currentIndex = 0;     // Initialize the current index to 0
    while (current != NULL) {  // Traverse the list
        if (currentIndex == index) {
            return current;     // Return the node if the index matches
        }
        current = current->tail; // Move to the next node
        currentIndex++;          // Increment the index
    }
    return NULL; // Return NULL if the index is out of bounds
}

ListNode* List_reverse(ListNode* list) {
    ListNode *prev = NULL; // Pointer to the previous node
    ListNode *current = list; // Pointer to the current node
    ListNode *next = NULL; // Pointer to the next node
    while (current != NULL) { // Traverse the list
        next = current->tail; // Store the next node
        current->tail = prev; // Reverse the link
        prev = current;       // Move prev to current
        current = next;       // Move to the next node
    }
    return prev; // New head of the reversed list
}

int List_length (ListNode* list) {
    if (!list) {
        return -1;
    }
    ListNode* current = list;
    int counter = 0;
    while (current) {
        counter++;
        current = current->tail;
    }
    return counter;
}

void List_free(ListNode *list) {
    ListNode *current = list; // Pointer to traverse the list
    while (current) { // Traverse the list until we reach the end
        ListNode *temp = current;       // Store the current node temporarily
        current = current->tail;         // Move to the next node
        free(temp->data);                // Free the data in the current node
        free(temp);                      // Free the current node itself
    }
}

void List_map (ListNode* list, void apply(void **x, void *cl), void *cl) {
    ListNode *current = list; // Start from the head of the list

    while (current != NULL) {  // Traverse the list
        apply(&(current->data), cl); // Apply the function to the data
        current = current->tail; // Move to the next node
    }
}