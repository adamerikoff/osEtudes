#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "darray.h"

DArray* DArray_new(int count, ...) {
    DArray *darr = (DArray *)malloc(sizeof(DArray));
    if (!darr) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    darr->capacity = (count > 0) ? count : INITSIZE;
    darr->length = 0;
    darr->array = (int *)malloc(darr->capacity * sizeof(int));
    if (!darr->array) {
        fprintf(stderr, "Memory allocation failed\n");
        free(darr);
        return NULL;
    }

    if (count > 0) {
        va_list args;
        va_start(args, count);
        for (int i = 0; i < count; i++) {
            darr->array[i] = va_arg(args, int);
            darr->length++;
        }
        va_end(args);
    }

    return darr;
}

int DArray_length(DArray *darr) {
    return darr->length;
}


int DArray_capacity(DArray *darr) {
    return darr->capacity;
}


int DArray_expand(DArray *darr, int new_capactiy) {
    if (new_capactiy <= darr->capacity) {
        return darr->capacity;
    }

    int *new_array = (int *)realloc(darr->array, new_capactiy * sizeof(int));
    if (!new_array) {
        fprintf(stderr, "Memory reallocation failed\n");
        return -1;
    }

    darr->array = new_array;
    darr->capacity = new_capactiy;
    return new_capactiy;
}

int DArray_get(DArray *darr, int index) {
    if (index < 0 || index >= darr->length) {
        fprintf(stderr, "Index out of bounds\n");
        return -1;
    }
    return darr->array[index];
}

int DArray_put(DArray *darr, int index, int value) {
    if (index >= darr->capacity) {
        int new_capacity = darr->capacity;
        while (new_capacity <= index) {
            new_capacity *= 2;
        }
        if (DArray_expand(darr, new_capacity) == -1) {
            return -1;
        }
    }
    if (index >= darr->length) {
        darr->length = index + 1;
    }
    darr->array[index] = value;
    return 0;
}

DArray* DArray_copy(DArray *darr) {
    DArray *new_darr = DArray_new(darr->length);
    if (!new_darr) {
        return NULL;
    }

    for (int i = 0; i < darr->length; i++) {
        new_darr->array[i] = darr->array[i];
    }
    new_darr->length = darr->length;
    return new_darr;
}

void DArray_free(DArray *darr) {
    if (darr) {
        if (darr->array) {
            free(darr->array);
        }
        free(darr);
    }
}

int DArray_next_prime(int n) {
    int i, j;
    for (i = n + 1;; i++) {
        for (j = 2; j * j <= i; j++) {
            if (i % j == 0) {
                break;
            }
        }
        if (j * j > i) {
            return i;
        }
    }
}