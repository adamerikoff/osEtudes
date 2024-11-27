#include "./headers/bitfield.h"
#include "./headers/memory.h"

#define CONVERT_CAPACITY(capacity) (((capacity) + 7) / 8)

void initBitfield(Bitfield* bitfield, size_t capacity) {
    bitfield->data = ALLOCATE(uint8_t, CONVERT_CAPACITY(capacity), true);
    if (bitfield->data == NULL) {
        fprintf(stderr, "Error: Out of memory while initializing bitfield.\n");
        exit(1);
    }
    bitfield->capacity = capacity;
}

// Free the memory used by the bitfield
void freeBitfield(Bitfield* bitfield) {
    FREE_ARRAY(uint8_t, bitfield->data, CONVERT_CAPACITY(bitfield->capacity));
    bitfield->data = NULL;
    bitfield->capacity = 0;
}

// Resize the bitfield to a new capacity (in bits)
void resizeBitfield(Bitfield* bitfield, size_t capacity) {
    size_t oldCapacity = CONVERT_CAPACITY(bitfield->capacity);
    size_t newCapacity = CONVERT_CAPACITY(capacity);

    bitfield->data = GROW_ARRAY_ZERO(bitfield->data, uint8_t, oldCapacity, newCapacity);
    if (bitfield->data == NULL) {
        fprintf(stderr, "Error: Out of memory while resizing bitfield.\n");
        exit(1);
    }
    if (newCapacity > oldCapacity) {
        memset(bitfield->data + oldCapacity, 0, newCapacity - oldCapacity);
    }

    bitfield->capacity = capacity;
}

void bitfieldSet(Bitfield* bitfield, size_t address) {
    bitfield->data[address >> 3] |= (uint8_t)(0x1 << (address & 0x7));
}

void bitfieldClear(Bitfield* bitfield, size_t address) {
    bitfield->data[address >> 3] &= (uint8_t)~(0x1 << (address & 0x7));
}

bool bitfieldGet(Bitfield* bitfield, size_t address) {
    return (bitfield->data[address >> 3] & (uint8_t)(0x1 << (address & 0x7))) != 0;
}
