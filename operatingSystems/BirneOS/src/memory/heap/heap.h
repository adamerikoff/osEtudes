#ifndef HEAP_H
#define HEAP_H

#include <stddef.h>
#include <stdint.h>
#include "../../config.h"
#include "../../kernel.h"
#include "../../status.h"
#include "../memory.h"


#define HEAP_BLOCK_TABLE_ENTRY_TAKEN 0x01
#define HEAP_BLOCK_TABLE_ENTRY_FREE 0x00

#define HEAP_BLOCK_HAS_NEXT 0b10000000
#define HEAP_BLOCK_IS_FIRST 0b01000000

typedef unsigned char HEAP_BLOCK_TABLE_ENTRY;

typedef struct heap_table {
    HEAP_BLOCK_TABLE_ENTRY* entries;
    size_t total;
} heap_table;

typedef struct heap {
    heap_table* table;
    // Start address of the heap data pool
    void* saddr;
} heap;

int heap_create(heap* h, void* ptr, void* end, heap_table* table);
void* heap_malloc(heap* h, size_t size);
void heap_free(heap* h, void* ptr);

#endif