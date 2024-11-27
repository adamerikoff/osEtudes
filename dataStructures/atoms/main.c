#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// Include the header file for the atom functions
#include "atom.h"

// Function to test the Atom functionality
void run_tests() {
    // Test 1: Creating atoms
    printf("Test 1: Creating atoms\n");
    
    const char *atom1 = Atom_string("hello");
    printf("Created atom1: '%s' at address %p\n", atom1, (void*)atom1);

    const char *atom2 = Atom_string("world");
    printf("Created atom2: '%s' at address %p\n", atom2, (void*)atom2);

    const char *atom3 = Atom_string("hello"); // Should return the same atom as atom1
    printf("Created atom3 (duplicate of atom1): '%s' at address %p\n", atom3, (void*)atom3);
    
    assert(atom1 == atom3); // Should be the same atom
    printf("Assertion passed: atom1 and atom3 point to the same memory at address %p.\n", (void*)atom1);

    // Test 2: Checking string values
    printf("\nTest 2: Checking string values\n");
    assert(strcmp(atom1, "hello") == 0);
    printf("Assertion passed: atom1 is 'hello'.\n");
    
    assert(strcmp(atom2, "world") == 0);
    printf("Assertion passed: atom2 is 'world'.\n");

    // Test 3: Length checking
    printf("\nTest 3: Length checking\n");
    assert(Atom_length("hello") == 5); // Length should be 5
    printf("Assertion passed: Length of 'hello' is 5.\n");

    assert(Atom_length("world") == 5); // Length should be 5
    printf("Assertion passed: Length of 'world' is 5.\n");

    assert(Atom_length("not_found") == 0); // Should return 0 for non-existent atom
    printf("Assertion passed: Length of 'not_found' is 0.\n");

    // Test 4: Integer atom creation
    printf("\nTest 4: Integer atom creation\n");
    const char *atom_int = Atom_int(12345);
    assert(strcmp(atom_int, "12345") == 0); // Check if the atom created for the integer is correct
    printf("Created atom_int: '%s' at address %p\n", atom_int, (void*)atom_int);
    
    assert(Atom_length("12345") == 5); // Length should be 5
    printf("Assertion passed: Length of '12345' is 5.\n");
    
    assert(Atom_length("hello") == 5);
    printf("Assertion passed: Length of 'hello' is 5.\n");
    
    assert(Atom_length("world") == 5); // "world" should still exist
    printf("Assertion passed: Length of 'world' is still 5.\n");

    printf("\nAll tests passed!\n");
}

int main() {
    // Run the tests
    run_tests();

    return 0;
}
