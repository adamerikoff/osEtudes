#include <stdio.h>
#include <assert.h>
#include "darray.h"

int main() {
    DArray *darr1 = DArray_new(3, 5, 10, 15);
    assert(darr1 != NULL);
    assert(DArray_length(darr1) == 3);
    assert(DArray_capacity(darr1) == 3);
    assert(DArray_get(darr1, 0) == 5);
    assert(DArray_get(darr1, 1) == 10);
    assert(DArray_get(darr1, 2) == 15);
    printf("Test 1 passed: Creation of dynamic array with initial values.\n");

    assert(DArray_put(darr1, 3, 20) == 0);
    assert(DArray_length(darr1) == 4);
    assert(DArray_get(darr1, 3) == 20);
    printf("Test 2 passed: Adding elements using DArray_put.\n");

    assert(DArray_put(darr1, 5, 25) == 0); 
    assert(DArray_get(darr1, 5) == 25);
    assert(DArray_length(darr1) == 6);
    assert(DArray_capacity(darr1) >= 6);
    printf("Test 3 passed: Capacity expansion when adding elements.\n");

    assert(DArray_get(darr1, 0) == 5);
    assert(DArray_get(darr1, 1) == 10);
    assert(DArray_get(darr1, 2) == 15);
    assert(DArray_get(darr1, 3) == 20);
    assert(DArray_get(darr1, 5) == 25);
    printf("Test 4 passed: Retrieving elements using DArray_get.\n");

    DArray *copy = DArray_copy(darr1);
    assert(copy != NULL);
    assert(DArray_length(copy) == DArray_length(darr1));
    assert(DArray_capacity(copy) == DArray_capacity(darr1));
    for (int i = 0; i < DArray_length(copy); i++) {
        assert(DArray_get(copy, i) == DArray_get(darr1, i));
    }
    printf("Test 5 passed: Copying the array using DArray_copy.\n");

    int new_capacity = 50;
    assert(DArray_expand(darr1, new_capacity) == new_capacity);
    assert(DArray_capacity(darr1) == new_capacity);
    printf("Test 6 passed: Expanding the array manually using DArray_expand.\n");

    assert(DArray_get(darr1, 100) == -1);
    printf("Test 7 passed: Handling out-of-bounds retrieval.\n");

    DArray_free(darr1);
    DArray_free(copy);
    printf("Test 8 passed: Freeing memory using DArray_free.\n");

    printf("All tests passed successfully!\n");
    return 0;
}
