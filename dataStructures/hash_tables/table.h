#ifndef TABLE_H
#define TABLE_H

#include <stdbool.h>

#define TABLE_SIZE 10

typedef struct Entry {
    int key;
    int value;
    struct Entry* next;
} Entry;

typedef struct {
    Entry* entries[TABLE_SIZE];
} HashTable;

// Function declarations
HashTable* HashTable_create_table();
void HashTable_free_table(HashTable* table);
unsigned int HashTable_hash_function(int key);
bool HashTable_insert(HashTable* table, int key, int value);
bool HashTable_search(HashTable* table, int key, int* value);
bool HashTable_delete(HashTable* table, int key);
void HashTable_print_table(HashTable* table);

#endif
