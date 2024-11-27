#ifndef CLOX_TABLE_H
#define CLOX_TABLE_H

#include "common.h"
#include "value.h"

typedef struct {
    bool present;
    Value key;
    Value value;
} Entry;

typedef struct {
    size_t capacityCount;
    size_t count;
    size_t capacity;
    Entry* entries;
} Table;

void initTable(Table* table);
void freeTable(Table* table);
bool tableGet(Table* table, Value key, Value* value);
bool tableSet(Table* table, Value key, Value value);
bool tableDelete(Table* table, Value key);
void tableAddAll(Table* src, Table* dest);
ObjString* tableFindString(Table* table, const char* chars, int length, uint32_t hash);
void tablePrint(Table* table);
size_t tableSize(Table* table);

#endif
