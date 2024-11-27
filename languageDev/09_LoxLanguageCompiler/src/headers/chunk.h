#ifndef CLOX_CHUNK_H
#define CLOX_CHUNK_H

#include "common.h"
#include "memory.h"
#include "value.h"
#include "table.h"

typedef enum {
    OP_CONSTANT,
    OP_CONSTANT_LONG,
    OP_DEFINE_GLOBAL,
    OP_DEFINE_GLOBAL_LONG,
    OP_DEFINE_GLOBAL_CONST,
    OP_DEFINE_GLOBAL_CONST_LONG,
    OP_GET_GLOBAL,
    OP_GET_GLOBAL_LONG,
    OP_SET_GLOBAL,
    OP_SET_GLOBAL_LONG,
    OP_GET_LOCAL,
    OP_GET_LOCAL_LONG,
    OP_SET_LOCAL,
    OP_SET_LOCAL_LONG,
    OP_JUMP,
    OP_JUMP_IF_FALSE,
    OP_LOOP,
    OP_NIL,
    OP_TRUE,
    OP_FALSE,
    OP_POP,
    OP_EQUAL,
    OP_GREATER,
    OP_LESS,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_NOT,
    OP_NEGATE,
    OP_PRINT,
    OP_RETURN
} OpCode;

typedef struct {
    size_t count;
    size_t capacity;
    uint8_t* code;
    size_t line_count;
    size_t line_capacity;
    uint16_t* lines;
    ValueArray constants;
#ifdef CLOX_CONST_CACHE
    Table constantTable;
#endif
} Chunk;

void initChunk(Chunk* chunk);
void freeChunk(Chunk* chunk);
int addConstant(Chunk* chunk, Value value);
int writeChunk(Chunk* chunk, uint8_t byte, size_t line);
int writeConstant(Chunk* chunk, Value value, size_t line);
size_t getLine(Chunk* chunk, size_t index);

#define CHUNK_SHORT_CONSTANTS 255
#define CHUNK_LONG_CONSTANTS 16777214

#ifdef CLOX_LONG_CONSTANTS
#define CHUNK_MAX_CONSTANTS CHUNK_LONG_CONSTANTS
#else
#define CHUNK_MAX_CONSTANTS CHUNK_SHORT_CONSTANTS
#endif

#endif
