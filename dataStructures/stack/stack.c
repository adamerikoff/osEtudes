#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "stack.h"

void Stack_init(Stack* stack) {
    stack->size = 0;
    stack->capacity = INIT_SIZE;
    stack->elements = (int*)malloc(stack->capacity * sizeof(int));
}

bool Stack_is_empty(Stack* stack) {
    return stack->size == 0;
}

void Stack_push(Stack* stack, int value) {
    if (stack->size == stack->capacity) {
        stack->capacity *= 2;
        stack->elements = (int*)realloc(stack->elements, stack->capacity * sizeof(int));
    }
    stack->elements[stack->size++] = value;
}

int Stack_pop(Stack* stack) {
    if (Stack_is_empty(stack)) {
        return -1;
    }
    return stack->elements[--stack->size];
}

int Stack_peak(Stack* stack) {
    if (Stack_is_empty(stack)) {
        return -1;
    }
    return stack->elements[stack->size - 1];
}

int Stack_next_prime(int n) {
    bool is_prime;
    while (true) {
        n++;
        is_prime = true;
        for (int i = 2; i * i <= n; i++) {
            if (n % i == 0) {
                is_prime = false;
                break;
            }
        }
        if (is_prime) {
            return n;
        }
    }
}

char* Stack_print(Stack* stack) {
    char* buffer = (char*)malloc(stack->size * 12 + 1);
    char* p = buffer;
    for (int i = 0; i < stack->size; i++) {
        p += sprintf(p, "%d ", stack->elements[i]);
    }
    return buffer;
}

void Stack_free(Stack* stack) {
    free(stack->elements);
}
