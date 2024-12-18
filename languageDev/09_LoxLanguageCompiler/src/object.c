#include "./headers/memory.h"
#include "./headers/object.h"
#include "./headers/value.h"

#define ALLOCATE_OBJ(vm, type, objectType) \
    (type*)allocateObject(vm, sizeof(type), objectType)

static Obj* allocateObject(FreeList* freeList, size_t size, ObjType type) {
    Obj* object = (Obj*)reallocate(NULL, 0, size, true);
    object->type = type;

    object->next = freeList->head;
    freeList->head = object;

    return object;
}

static ObjString* allocateString(FreeList* freeList, Table* strings, char* chars, int length,
        uint32_t hash) {
    ObjString* string = ALLOCATE_OBJ(freeList, ObjString, OBJ_STRING);
    string->length = length;
    string->chars = chars;
    string->hash = hash;

    tableSet(strings, OBJ_VAL(string), NIL_VAL);

    return string;
}

ObjString* takeString(FreeList* freeList, Table* strings, char* chars, int length) {
    uint32_t hash = hashString(chars, length);
    ObjString* interned = tableFindString(strings, chars, length, hash);
    if (interned != NULL) {
        FREE_ARRAY(char, chars, length + 1);
        return interned;
    }

    return allocateString(freeList, strings, chars, length, hash);
}

ObjString* copyString(FreeList* freeList, Table* strings, const char* chars, int length) {
    uint32_t hash = hashString(chars, length);
    ObjString* interned = tableFindString(strings, chars, length, hash);

    if (interned != NULL) {
        return interned;
    }

    char* heapChars = ALLOCATE(char, length + 1, false);
    if (heapChars == NULL) {
        // TODO out of memory
        return NULL;
    }
    memcpy(heapChars, chars, (size_t)length);
    heapChars[length] = '\0';

    return allocateString(freeList, strings, heapChars, length, hash);
}

void printObject(Value value) {
    switch (OBJ_TYPE(value)) {
        case OBJ_STRING:
            printf("%s", AS_CSTRING(value));
            break;
    }
}

void printObjectType(Value value) {
    switch (OBJ_TYPE(value)) {
        case OBJ_STRING:
            printf("%-4s", "STR");
            break;
    }
}
