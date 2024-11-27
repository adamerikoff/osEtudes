#ifndef SET_INCLUDED
#define SET_INCLUDED

#include <stdarg.h>
#include <stdbool.h>

#define INITSIZE 20

typedef struct Set {
    int length;
    int elements[INITSIZE];
} Set;

extern Set* Set_new(int count, ...);
extern Set* Set_add(Set* set, int element);
extern Set* Set_remove(Set* set, int element);
extern bool Set_contains(Set* set, int element);
extern Set* Set_union(Set* set1, Set* set2);
extern Set* Set_intersection(Set* set1, Set* set2);
extern Set* Set_difference(Set* set1, Set* set2);
extern bool Set_issubset(Set* set1, Set* set2);
extern int Set_size(Set* set);
extern Set* Set_clear(Set* set);


#endif