#ifndef CLOX_BITFIELD_H
#define CLOX_BITFIELD_H

#include "common.h"

typedef struct {
    size_t capacity;
    uint8_t *data;
} Bitfield;

void initBitfield(Bitfield* bitfield, size_t capacity);
void freeBitfield(Bitfield* bitfield);
void resizeBitfield(Bitfield* bitfield, size_t capacity);
void bitfieldSet(Bitfield* bitfield, size_t address);
void bitfieldClear(Bitfield* bitfield, size_t address);
bool bitfieldGet(Bitfield* bitfield, size_t address);

#endif
