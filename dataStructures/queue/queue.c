#include "queue.h"
#include <stdio.h>
#include <stdlib.h>

void Queue_init(Queue* q) {
    q->front = NULL;
    q->rear = NULL;
}

bool Queue_is_empty(Queue* q) {
    return q->front == NULL;
}

void Queue_enqueue(Queue* q, int value) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    new_node->data = value;
    new_node->next = NULL;

    if (Queue_is_empty(q)) {
        q->front = new_node;
    } else {
        q->rear->next = new_node;
    }
    q->rear = new_node;
}

int Queue_dequeue(Queue* q) {
    if (Queue_is_empty(q)) {
        return -1; // Queue is empty
    }
    Node* temp = q->front;
    int value = temp->data;
    q->front = q->front->next;
    if (q->front == NULL) {
        q->rear = NULL;
    }
    free(temp);
    return value;
}

int Queue_peek(Queue* q) {
    if (Queue_is_empty(q)) {
        return -1; // Queue is empty
    }
    return q->front->data;
}

void Queue_free(Queue* q) {
    while (!Queue_is_empty(q)) {
        Queue_dequeue(q);
    }
}
