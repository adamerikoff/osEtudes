#include <stdio.h>
#include <stdlib.h> // For malloc
#include <string.h> // For memcpy and strdup
#include <stdarg.h> // For va_list

#ifndef LIST_INCLUDED
#define LIST_INCLUDED

typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STRING
} DataType;

typedef struct ListNode {
    DataType type;             // Type of the data stored
    void *data;                // Pointer to the data
    struct ListNode *tail;     // Pointer to the next node
} ListNode;

extern void List_print(ListNode *list);
extern ListNode* createNode(DataType type, void *value);
extern ListNode* List_list(DataType firstType, void *firstValue, ...);
extern ListNode* List_append(ListNode* node, ListNode* newNode);
extern ListNode* List_copy(ListNode* list);
extern ListNode* List_get (ListNode* list, int index);
extern ListNode* List_reverse(ListNode* list);
extern int List_length (ListNode* list);
extern void List_free (ListNode *list);
extern void List_map (ListNode* list, void apply(void **x, void *cl), void *cl);

#endif