#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "../heap/kheap.h"
#include "../../status.h"

#define PAGING_CACHE_DISABLED  0b00010000
#define PAGING_WRITE_THROUGH   0b00001000
#define PAGING_ACCESS_FROM_ALL 0b00000100
#define PAGING_IS_WRITABLE     0b00000010
#define PAGING_IS_PRESENT      0b00000001

#define PAGING_TOTAL_ENTRIES_PER_TABLE 1024
#define PAGING_PAGE_SIZE 4096

typedef struct {
    uint32_t* directory_entry;
} paging_4gb_chunk;

extern void paging_load_directory(uint32_t* directory);
extern void enable_paging();

paging_4gb_chunk* paging_new_4gb(uint8_t flags);
void paging_switch(uint32_t* directory);
uint32_t* paging_4gb_chunk_get_directory(paging_4gb_chunk* chunk);

int paging_set(uint32_t* directory, void* virtual, uint32_t val);
bool paging_is_aligned(void* addr);

#endif