#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "set.h"

// Function to print the contents of a set
void print_set(Set* set) {
    printf("{ ");
    for (int i = 0; i < set->length; i++) {
        printf("%d ", set->elements[i]);
    }
    printf("}\n");
}

// Test function to run tests and log results
void run_tests() {
    printf("Running tests...\n");

    // Test Set_new
    printf("Test: Set_new\n");
    Set* set1 = Set_new(3, 1, 2, 3);
    printf("Created set1: ");
    print_set(set1);

    // Test Set_contains
    printf("Test: Set_contains\n");
    printf("Contains 2 in set1: %s\n", Set_contains(set1, 2) ? "true" : "false");
    printf("Contains 4 in set1: %s\n", Set_contains(set1, 4) ? "true" : "false");

    // Test Set_add
    printf("Test: Set_add\n");
    Set_add(set1, 4);
    printf("After adding 4 to set1: ");
    print_set(set1);

    // Test Set_remove
    printf("Test: Set_remove\n");
    Set_remove(set1, 2);
    printf("After removing 2 from set1: ");
    print_set(set1);

    // Test Set_union
    printf("Test: Set_union\n");
    Set* set2 = Set_new(3, 3, 4, 5);
    printf("Created set2: ");
    print_set(set2);
    Set* union_set = Set_union(set1, set2);
    printf("Union of set1 and set2: ");
    print_set(union_set);

    // Test Set_intersection
    printf("Test: Set_intersection\n");
    Set* intersection_set = Set_intersection(set1, set2);
    printf("Intersection of set1 and set2: ");
    print_set(intersection_set);

    // Test Set_difference
    printf("Test: Set_difference\n");
    Set* difference_set = Set_difference(set1, set2);
    printf("Difference of set1 and set2 (set1 - set2): ");
    print_set(difference_set);

    // Test Set_issubset
    printf("Test: Set_issubset\n");
    printf("Is set1 a subset of set2? %s\n", Set_issubset(set1, set2) ? "true" : "false");
    printf("Is set2 a subset of set1? %s\n", Set_issubset(set2, set1) ? "true" : "false");

    // Test Set_clear
    printf("Test: Set_clear\n");
    Set_clear(set1);
    printf("After clearing set1: ");
    print_set(set1);

    // Clean up allocated memory
    free(set1);
    free(set2);
    free(union_set);
    free(intersection_set);
    free(difference_set);
}

int main() {
    run_tests();
    return 0;
}
