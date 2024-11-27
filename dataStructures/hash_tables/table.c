#include <stdio.h>
#include <stdlib.h>
#include "table.h"

HashTable* HashTable_create_table() {
    HashTable* table = (HashTable*)malloc(sizeof(HashTable));
    for (int i = 0; i < TABLE_SIZE; i++) {
        table->entries[i] = NULL;
    }
    return table;
}

void HashTable_free_table(HashTable* table) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        Entry* entry = table->entries[i];
        while (entry) {
            Entry* temp = entry;
            entry = entry->next;
            free(temp);
        }
    }
    free(table);
}

unsigned int HashTable_hash_function(int key) {
    return key % TABLE_SIZE;
}

bool HashTable_insert(HashTable* table, int key, int value) {
    unsigned int index = HashTable_hash_function(key);
    Entry* new_entry = (Entry*)malloc(sizeof(Entry));
    if (new_entry == NULL) return false;

    new_entry->key = key;
    new_entry->value = value;
    new_entry->next = NULL;

    if (table->entries[index] == NULL) {
        table->entries[index] = new_entry;
    } else {
        Entry* current = table->entries[index];
        while (current->next != NULL) {
            if (current->key == key) {
                current->value = value;
                free(new_entry);
                return true;
            }
            current = current->next;
        }
        if (current->key == key) {
            current->value = value;
            free(new_entry);
        } else {
            current->next = new_entry;
        }
    }
    return true;
}

bool HashTable_search(HashTable* table, int key, int* value) {
    unsigned int index = HashTable_hash_function(key);
    Entry* current = table->entries[index];

    while (current != NULL) {
        if (current->key == key) {
            *value = current->value;
            return true;
        }
        current = current->next;
    }
    return false;
}

bool HashTable_delete(HashTable* table, int key) {
    unsigned int index = HashTable_hash_function(key);
    Entry* current = table->entries[index];
    Entry* prev = NULL;

    while (current != NULL) {
        if (current->key == key) {
            if (prev == NULL) {
                table->entries[index] = current->next;
            } else {
                prev->next = current->next;
            }
            free(current);
            return true;
        }
        prev = current;
        current = current->next;
    }
    return false;
}

void HashTable_print_table(HashTable* table) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        printf("Index %d: ", i);
        Entry* current = table->entries[i];
        while (current != NULL) {
            printf(" -> [Key: %d, Value: %d]", current->key, current->value);
            current = current->next;
        }
        printf(" -> NULL\n");
    }
}
