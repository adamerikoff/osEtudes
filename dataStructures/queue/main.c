#include <stdio.h>
#include "queue.h"

void log(const char* message) {
    printf("LOG: %s\n", message);
}

void test_queue() {
    Queue q;
    Queue_init(&q);

    log("Testing enqueue operation.");
    Queue_enqueue(&q, 10);
    Queue_enqueue(&q, 20);
    Queue_enqueue(&q, 30);

    log("Testing peek operation.");
    int front = Queue_peek(&q);
    printf("Front element: %d\n", front);

    log("Testing dequeue operation.");
    int dequeued = Queue_dequeue(&q);
    printf("Dequeued: %d\n", dequeued);

    log("Testing peek operation after dequeue.");
    front = Queue_peek(&q);
    printf("Front element after dequeue: %d\n", front);

    log("Freeing the queue.");
    Queue_free(&q);
}

int main() {
    log("Starting Queue Tests.");
    test_queue();
    log("All tests completed.");
    return 0;
}
