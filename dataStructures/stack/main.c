#include <stdio.h>
#include <assert.h>
#include "stack.h"

void log(const char* message) {
    printf("%s\n", message);
}

void test_stack() {
    Stack stack;
    Stack_init(&stack);
    
    log("Testing Stack Initialization");
    assert(Stack_is_empty(&stack) == true);
    log("Stack is initialized and is empty.");

    log("Pushing 5 onto the stack");
    Stack_push(&stack, 5);
    assert(Stack_is_empty(&stack) == false);
    assert(Stack_peak(&stack) == 5);
    log("Top element is now 5.");

    log("Pushing 10 onto the stack");
    Stack_push(&stack, 10);
    log("Pushing 15 onto the stack");
    Stack_push(&stack, 15);
    assert(Stack_peak(&stack) == 15);
    log("Top element is now 15.");

    log("Popping from the stack");
    assert(Stack_pop(&stack) == 15);
    assert(Stack_peak(&stack) == 10);
    log("Popped 15, top element is now 10.");

    log("Pushing 20 onto the stack");
    Stack_push(&stack, 20);
    log("Pushing 25 onto the stack");
    Stack_push(&stack, 25);
    assert(Stack_peak(&stack) == 25);
    log("Top element is now 25.");

    char* contents = Stack_print(&stack);
    assert(contents != NULL);
    log("Current Stack contents:");
    printf("%s\n", contents);
    free(contents);

    log("Popping elements from the stack");
    assert(Stack_pop(&stack) == 25);
    log("Popped 25.");
    assert(Stack_pop(&stack) == 20);
    log("Popped 20.");
    assert(Stack_pop(&stack) == 10);
    log("Popped 10.");
    assert(Stack_pop(&stack) == 5);
    log("Popped 5.");
    assert(Stack_pop(&stack) == -1);
    log("Attempted to pop from an empty stack.");

    assert(Stack_is_empty(&stack) == true);
    log("Stack is empty after all pops.");

    Stack_free(&stack);
    log("Stack memory has been freed.");
}

int main() {
    log("Starting stack tests.");
    test_stack();
    log("All tests passed!");
    return 0;
}
