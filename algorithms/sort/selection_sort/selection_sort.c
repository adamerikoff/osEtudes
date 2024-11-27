#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 1000000

// Function to perform Selection Sort
void selection_sort(int arr[], int n) {
    int i, j, min_idx, temp;
    for (i = 0; i < n - 1; i++) {
        min_idx = i;
        for (j = i + 1; j < n; j++) {
            if (arr[j] < arr[min_idx]) {
                min_idx = j;
            }
        }

        if (min_idx != i) {
            temp = arr[i];
            arr[i] = arr[min_idx];
            arr[min_idx] = temp;
        }
    }
}

int main(int argc, char* argv[]) {
    int list[N] = {};
    srand(time(NULL));

    // Fill the array with random numbers
    for (int i = 0; i < N; i++) {
        list[i] = rand() % 100000;
    }

    // Print the array before sorting (optional)
    printf("Array before sorting:\n");
    for (int i = 0; i < 20; i++) {  // Print first 20 elements for brevity
        printf("%d ", list[i]);
    }
    printf("\n");

    clock_t start_time = clock();
    // Perform selection sort
    selection_sort(list, N);

    clock_t end_time = clock();
    double time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("\n[SELECTION]Time taken to sort array: %.9f seconds\n", time_spent);

    // Print the array after sorting (optional)
    printf("Array after sorting:\n");
    for (int i = 0; i < 20; i++) {  // Print first 20 elements for brevity
        printf("%d ", list[i]);
    }
    printf("\n");

    return 0;
}
