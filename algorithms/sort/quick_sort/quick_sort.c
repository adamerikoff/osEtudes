#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 900000

int partition(int start, int end, int array[]) {
    int pivot = array[end];  // Use the last element as the pivot
    int i = start - 1;  // Index of smaller element

    for (int j = start; j < end; j++) {
        if (array[j] <= pivot) {
            i++;
            // Swap array[i] and array[j]
            int temp = array[i];
            array[i] = array[j];
            array[j] = temp;
        }
    }
    // Swap array[i+1] and array[end] (or pivot)
    int temp = array[i + 1];
    array[i + 1] = array[end];
    array[end] = temp;

    return i + 1;
}

void quick_sort(int start, int end, int array[]) {
    if (end <= start) {
        return;
    }
    int pivot = partition(start, end, array);
    quick_sort(start, pivot - 1, array);
    quick_sort(pivot + 1, end, array);
}

int main(int argc, char* argv[]) {
    int array[N] = {};
    srand(time(NULL));

    // Fill the array with random numbers
    for (int i = 0; i < N; i++) {
        array[i] = rand() % N;
    }
    // Print the array before sorting (optional)
    printf("Array before sorting:\n");
    for (int i = 0; i < 50; i++) {  // Print first 20 elements for brevity
        printf("%d ", array[i]);
    }
    printf("\n");

    clock_t start_time = clock();
    // Perform selection sort
    quick_sort(0, N - 1, array);
    clock_t end_time = clock();
    double time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("\n[QUICK]Time taken to sort array: %.9f seconds\n", time_spent);

    // Print the array after sorting (optional)
    printf("Array after sorting:\n");
    for (int i = 0; i < 50; i++) {  // Print first 20 elements for brevity
        printf("%d ", array[i]);
    }
    printf("\n");

    return 0;
}
