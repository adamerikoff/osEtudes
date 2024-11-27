#include <stdarg.h>

#ifndef DARRAY_INCLUDED
#define DARRAY_INCLUDED

#define INITSIZE 23

typedef struct DArray {
    int length;
    int capacity;
    int *array;
} DArray;

extern DArray* DArray_new(int count, ...);
extern int DArray_expand(DArray *darr, int new_capacity);
extern int DArray_get(DArray *darr, int index);
extern int DArray_put(DArray *darr, int index, int value);
extern DArray* DArray_copy(DArray *darr);
extern void DArray_free(DArray *darr);
extern int DArray_next_prime(int n);
extern int DArray_length(DArray *darr);
extern int DArray_capacity(DArray *darr);

#endif