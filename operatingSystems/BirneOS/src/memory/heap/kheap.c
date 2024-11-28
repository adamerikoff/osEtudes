#include "kheap.h"

heap kernel_heap;
heap_table kernel_heap_table;

void kheap_init() {
    int total_table_entries = BIRNEOS_HEAP_SIZE_BYTES / BIRNEOS_HEAP_BLOCK_SIZE;
    kernel_heap_table.entries = (HEAP_BLOCK_TABLE_ENTRY*)(BIRNEOS_HEAP_TABLE_ADDRESS);
    kernel_heap_table.total = total_table_entries;
    void* end = (void*)(BIRNEOS_HEAP_ADDRESS + BIRNEOS_HEAP_SIZE_BYTES);
    int res = heap_create(&kernel_heap, (void*)(BIRNEOS_HEAP_ADDRESS), end, &kernel_heap_table);
    if (res < 0) {
        print("FAILED TO CREATE HEAP!\n");
    }
}

void* kmalloc(size_t size) {
    return heap_malloc(&kernel_heap, size);
}

void kfree(void* ptr) {
    heap_free(&kernel_heap, ptr);
}

void* kzalloc(size_t size) {
    void* ptr = kmalloc(size);
    if (!ptr) {
        return 0;
    }
    memset(ptr, 0x00, size);
    return ptr;
}