#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Define the struct Row
typedef struct {
    int id;
    char name[20];  // Each name will have up to 20 characters
} Row;

// Function to generate a random name
void generate_random_name(char* name, int length) {
    static const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    for (int i = 0; i < length - 1; ++i) {
        int random_index = rand() % (sizeof(charset) - 1);
        name[i] = charset[random_index];
    }
    name[length - 1] = '\0';  // Null-terminate the string
}

// Function to generate 100000 rows
void generate_rows(Row* rows, int count) {
    for (int i = 0; i < count; ++i) {
        rows[i].id = 1000 + i;  // Start id from 1000
        generate_random_name(rows[i].name, 20);  // Generate a random name of 20 characters
    }
}

int binary_search(Row* rows, int n, int target) {
    int left = 0;
    int right = n;
    int mid = 0;

    while (left <= right) {
        mid = left + (right - left) / 2;
        if (rows[mid].id == target) {
            return rows[mid].id;
        } else if (rows[mid].id < target) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return -1;
}

int brute_force(Row* rows, int n, int target) {
    for (int i = 0; i < n; i++) {
        if (rows[i].id == target) {
            return i;
        }
    }
    return -1;
}

int main(int argc, char *argv[]) {
    int count = 100000;
    Row* rows = malloc(count * sizeof(Row));
    if (rows == NULL) {
        printf("Memory allocation failed!\n");
        return 1;
    }
    srand(time(NULL));
    generate_rows(rows, count);

    // Binary search
    clock_t start_time_binary = clock();
    int idx_binary = binary_search(rows, count, 48930);
    clock_t end_time_binary = clock();
    double time_spent_binary = (double)(end_time_binary - start_time_binary) / CLOCKS_PER_SEC;
    if (idx_binary != -1) {
        printf("FOUND!: Row %d: ID = %d, Name = %s\n\n\n", idx_binary + 1, rows[idx_binary].id, rows[idx_binary].name);
        for (int i = idx_binary-5; i < idx_binary+5; ++i) {
            printf("Row %d: ID = %d, Name = %s\n", i + 1, rows[i].id, rows[i].name);
        }
    } else {
        printf("GROUND ZERO!");
    }
    printf("\n[BINARY]Time taken to find the ID %d: %.9f seconds\n", idx_binary, time_spent_binary);

    // Brute force
    clock_t start_time_brute = clock();
    int idx_brute = brute_force(rows, count, 48930);
    clock_t end_time_brute = clock();
    double time_spent_brute = (double)(end_time_brute - start_time_brute) / CLOCKS_PER_SEC;
    if (idx_brute != -1) {
        printf("FOUND!: Row %d: ID = %d, Name = %s\n\n\n", idx_brute + 1, rows[idx_brute].id, rows[idx_brute].name);
        for (int i = idx_brute-5; i < idx_brute+5; ++i) {
            printf("Row %d: ID = %d, Name = %s\n", i + 1, rows[i].id, rows[i].name);
        }
    } else {
        printf("GROUND ZERO!");
    }
    printf("\n[BRUTE]Time taken to find the ID %d: %.9f seconds\n", idx_brute, time_spent_brute);

    // Free the allocated memory
    free(rows);

    return 0;
}
