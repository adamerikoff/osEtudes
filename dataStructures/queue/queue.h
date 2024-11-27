#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>

typedef struct Node {
    int data;
    struct Node* next;
} Node;

typedef struct Queue {
    Node* front;
    Node* rear;
} Queue;

void Queue_init(Queue* q);
bool Queue_is_empty(Queue* q);
void Queue_enqueue(Queue* q, int value);
int Queue_dequeue(Queue* q);
int Queue_peek(Queue* q);
void Queue_free(Queue* q);

#endif // QUEUE_H
