#ifndef STACK_INCLUDED
#define STACK_INCLUDED

#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>

#define INIT_SIZE 5

typedef struct Stack {
    int *elements;
    int size;
    int capacity;
} Stack;

extern void Stack_init(Stack* stack);
extern bool Stack_is_empty(Stack* stack);
extern void Stack_push(Stack* stack, int value);
extern int Stack_pop(Stack* stack);
extern int Stack_peak(Stack* stack);
extern int Stack_next_prime(int n);
extern char* Stack_print(Stack* stack);
extern void Stack_free(Stack* stack);

#endif
