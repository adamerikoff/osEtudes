// main.c
#include <stdio.h>
#include "table.h"

void log(const char* message) {
    printf("[LOG]: %s\n", message);
}

int main() {
    // Create a hash table
    log("Creating a hash table...");
    HashTable* table = HashTable_create_table();

    // Insert some key-value pairs
    log("Inserting key-value pairs...");
    HashTable_insert(table, 1, 100);
    HashTable_insert(table, 2, 200);
    HashTable_insert(table, 12, 300);
    HashTable_insert(table, 22, 400);
    HashTable_insert(table, 3, 500);

    // Print the hash table
    log("Hash table contents:");
    HashTable_print_table(table);

    // Search for a key
    log("Searching for a key...");
    int value;
    if (HashTable_search(table, 12, &value)) {
        printf("Found key 12 with value: %d\n", value);
    } else {
        printf("Key 12 not found.\n");
    }

    // Delete a key
    log("Deleting a key...");
    HashTable_delete(table, 2);
    HashTable_print_table(table);

    // Free memory
    log("Freeing the hash table...");
    HashTable_free_table(table);

    return 0;
}
