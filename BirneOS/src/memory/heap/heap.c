#include "heap.h"

static int heap_validate_allignment(void* ptr) {
    return ((unsigned int)ptr % BIRNEOS_HEAP_BLOCK_SIZE) == 0;
}

static int heap_validate_table(void* ptr, void* end, heap_table* table) {
    int res = 0;

    size_t table_size = (size_t)(end - ptr);
    size_t total_blocks = table_size / BIRNEOS_HEAP_BLOCK_SIZE;
    if(table->total != total_blocks) {
        res = -EINVARG;
        goto out;
    }

out:
    return res;
}

static uint32_t heap_align_value_to_upper(uint32_t val) {
    if ((val % BIRNEOS_HEAP_BLOCK_SIZE) == 0) {
        return val;
    }
    val = (val - (val % BIRNEOS_HEAP_BLOCK_SIZE));
    val += BIRNEOS_HEAP_BLOCK_SIZE;
    return val;
}

int heap_create(heap* h, void* ptr, void* end, heap_table* table) {
    int res = 0;

    if (!heap_validate_allignment(ptr) || !heap_validate_allignment(end)) {
        res = -EINVARG;
        goto out;
    }

    memset(h, 0, sizeof(heap));
    h->saddr = ptr;
    h->table = table;

    res = heap_validate_table(ptr, end, table);
    if (res < 0) {
        goto out;
    }

    size_t table_size = sizeof(HEAP_BLOCK_TABLE_ENTRY) * table->total;
    memset(table->entries, HEAP_BLOCK_TABLE_ENTRY_FREE, table_size);
out:
    return res;
}

static int heap_get_entry_type(HEAP_BLOCK_TABLE_ENTRY entry) {
    return entry & 0x0f;
}

int heap_get_start_block(heap* h, uint32_t total_blocks) {
    heap_table* table = h->table;
    int bc = 0;
    int bs = -1;
    for (size_t i = 0; i < table->total; i++) {
        if (heap_get_entry_type(table->entries[i]) != HEAP_BLOCK_TABLE_ENTRY_FREE) {
            bc = 0;
            bs = -1;
            continue;
        }
        if (bs == -1) {
            bs = i;
        }
        bc++;
        if (bc == total_blocks) {
            break;
        }
    }
    if (bs == -1) {
        return -ENOMEM;
    }
    return bs;
}

void* heap_block_to_address(heap* h, int block) {
    return h->saddr + (block + BIRNEOS_HEAP_BLOCK_SIZE);
}

void heap_mark_blocks_taken(heap* h, int start_block, int total_blocks) {
    int end_block = (start_block + total_blocks) - 1;
    HEAP_BLOCK_TABLE_ENTRY entry = HEAP_BLOCK_TABLE_ENTRY_TAKEN | HEAP_BLOCK_IS_FIRST;
    if (total_blocks > 1) {
        entry |= HEAP_BLOCK_HAS_NEXT;
    }
    for (int i = start_block; i <= end_block; i++) {
        h->table->entries[i] = entry;
        entry = HEAP_BLOCK_TABLE_ENTRY_TAKEN;
        if (i != end_block - 1) {
            entry |= HEAP_BLOCK_HAS_NEXT;
        }
        
    }
}

void* heap_malloc_blocks(heap* h, uint32_t total_blocks) {
    void* address = 0;

    int start_block = heap_get_start_block(h, total_blocks);
    if (start_block < 0) {
        goto out;
    }
    address = heap_block_to_address(h, start_block);
    heap_mark_blocks_taken(h, start_block, total_blocks);
out:
    return address;
}

void* heap_malloc(heap* h, size_t size) {
    size_t aligned_size = heap_align_value_to_upper(size);
    uint32_t total_blocks = aligned_size / BIRNEOS_HEAP_BLOCK_SIZE;
    return heap_malloc_blocks(h, total_blocks);
}

int heap_address_to_block(heap* h, void* address) {
    return ((int)(address - h->saddr)) / BIRNEOS_HEAP_BLOCK_SIZE;
}

void heap_mark_blocks_free(heap* h, int starting_block) {
    heap_table* table = h->table;
    for (int i = starting_block; i < (int)table->total; i++) {
        HEAP_BLOCK_TABLE_ENTRY entry = table->entries[i];
        table->entries[i] = HEAP_BLOCK_TABLE_ENTRY_FREE;
        if (!(entry & HEAP_BLOCK_HAS_NEXT)) {
            break;
        }
    }
    
}

void heap_free(heap* h, void* ptr) {
    heap_mark_blocks_free(h, heap_address_to_block(h, ptr));
}