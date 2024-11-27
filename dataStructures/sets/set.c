#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>

#include "set.h"

Set* Set_new(int count, ...) {
    Set* set = (Set*)malloc(sizeof(Set));
    if (!set) {
        return NULL;
    }
    set->length = 0;

    va_list args;
    va_start(args, count);
    for (int i = 0; i < count && set->length < INITSIZE; i++) {
        int element = va_arg(args, int);
        set->elements[set->length++] = element;
    }
    va_end(args);

    return set;
}

Set* Set_add(Set* set, int element) {
    if (!Set_contains(set, element)) {
        set->elements[set->length] = element;
        set->length++;
    }
    return set;
}

Set* Set_remove(Set* set, int element) {
    if (Set_contains(set, element)) {
        for (int i = 0; i < set->length; i++) {
            if (set->elements[i] == element) {
                for (int j = i; j < set->length - 1; j++) {
                    set->elements[j] = set->elements[j + 1];
                }
                set->length--;
                break;
            }
        }
    }
    return set;
}

bool Set_contains(Set* set, int element) {
    for (int i = 0; i < set->length; i++)
    {
        if (element == set->elements[i]) {
            return true;
        }
    }
    return false;
}

Set* Set_union(Set* set1, Set* set2) {
    Set* result = (Set*)malloc(sizeof(Set));
    if (!result) {
        return NULL;
    }
    result->length = 0;
    for (int i = 0; i < set1->length; i++) {
        result->elements[result->length++] = set1->elements[i];
    }
    for (int i = 0; i < set2->length; i++) {
        if (!Set_contains(result, set2->elements[i])) {
            result->elements[result->length++] = set2->elements[i];
        }
    }
    return result;
}


Set* Set_intersection(Set* set1, Set* set2) {
    Set* result = (Set*)malloc(sizeof(Set));
    if (!result) {
        return NULL;
    }
    result->length = 0;
    for (int i = 0; i < set1->length; i++) {
        if (Set_contains(set2, set1->elements[i])) {
            result->elements[result->length++] = set1->elements[i];
        }
    }
    return result;
}


Set* Set_difference(Set* set1, Set* set2) {
    Set* result = (Set*)malloc(sizeof(Set));
    if (!result) {
        return NULL;
    }
    result->length = 0;
    for (int i = 0; i < set1->length; i++) {
        if (!Set_contains(set2, set1->elements[i])) {
            result->elements[result->length++] = set1->elements[i];
        }
    }
    return result;
}


bool Set_issubset(Set* set1, Set* set2) {
    for (int i = 0; i < set1->length; i++) {
        if (!Set_contains(set2, set1->elements[i])) {
            return false;
        }
    }
    return true;
}



int Set_size(Set* set) {
    return set->length;
}

Set* Set_clear(Set* set) {
    set->length = 0;  
    return set;
}