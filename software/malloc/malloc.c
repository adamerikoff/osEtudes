#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static uint8_t MEMORY_POOL[64];

typedef struct {
    void* ptr;
    uint64_t size;
} free_entry_t;

static free_entry_t FREE_LIST[64] = {
    (free_entry_t){
        .ptr = MEMORY_POOL,
        .size = 64
    },
};

static uint64_t FREE_LIST_USED = 1;

void print_free_list() {
    printf("FREE LIST:\n");
    for (uint64_t i = 0; i < FREE_LIST_USED; i++) {
        free_entry_t* entry = &FREE_LIST[i];
        printf("  %p   (%llu)\n", entry->ptr, entry->size);
    }
}

free_entry_t* find_free_entry(size_t size) {
    for (uint64_t i = 0; i < FREE_LIST_USED; i++) {
        free_entry_t* entry = &FREE_LIST[i];
        if (entry->size >= size) {
            return entry;
        }
    }
    abort();
}

void* custom_malloc(size_t size) {
    size += 8;

    free_entry_t *entry = find_free_entry(size);

    void* base_ptr = entry->ptr;
    uint64_t* size_ptr = base_ptr;
    void* user_ptr = base_ptr + 8;

    *size_ptr = size;

    entry->ptr += size;
    entry->size -=size;

    print_free_list();

    return user_ptr;
}

void custom_free(void* user_ptr) {
    free_entry_t* entry = &FREE_LIST[FREE_LIST_USED];

    void* base_ptr = user_ptr - 8;
    uint64_t* size_ptr = base_ptr;
    uint64_t size = *size_ptr;

    entry->ptr = base_ptr;
    entry->size = size;
    FREE_LIST_USED++;
    printf("FREE\n");
    print_free_list();
}

int main() {
    print_free_list();

    char* a;
    char* b;
    char* c;

    a = custom_malloc(4);
    b = custom_malloc(4);
    c = custom_malloc(4);

    strcpy(a, "foo\b");
    strcpy(b, "bar\b");
    strcpy(c, "baz\b");

    printf("%p\n", a);
    printf("%p\n", b);
    printf("%p\n", c);

    custom_free(b);
    char* d = custom_malloc(4);
    strcpy(d, "baz\b");
    printf("%p\n", d);
}