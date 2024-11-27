#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <limits.h>
#include <stdio.h>

#include "atom.h"

#define NELEMS(x) ((sizeof (x))/(sizeof ((x)[0])))

static struct atom {
    struct atom *link;
    int length;
    char *string;
} *buckets[2048];


unsigned long hash_string(const char *string) {
    unsigned long hash = 5381;  // Starting value for the hash
    int c;

    while ((c = *string++)) {  // While the current character is not null
        hash = ((hash << 5) + hash) + c;  // hash * 33 + c
    }

    return hash;  // Return the final computed hash value
}

int Atom_length(const char *string) {
    assert(string);  // Ensure the input string is not NULL.
    for (int i = 0; i < NELEMS(buckets); i++) { // Loop through all hash table buckets.
        for (struct atom *p = buckets[i]; p != NULL; p = p->link) { // Loop through each linked list in the bucket.
            // Compare the lengths and content of the strings.
            if (p->length == strlen(string) && strcmp(p->string, string) == 0) {  
                return p->length;  // Return the length of the string.
            }
        }
    }
    return 0;  // If the string is not found, return 0.
}


const char *Atom_new(const char *string, int length) {
    assert(string);
    assert(length >= 0);

    unsigned long h = hash_string(string) % NELEMS(buckets);

    for (struct atom *atom = buckets[h]; atom != NULL; atom = atom->link) {
        if (length == atom->length && strcmp(string, atom->string) == 0) {
            return atom->string;  // Return existing atom if found.
        }
    }
    // allocate memory and create new atom
    struct atom *new_atom = malloc(sizeof(struct atom));
    assert(new_atom);

    new_atom->length = length;
    new_atom->string = malloc((length + 1) * sizeof(char));  // +1 for the null terminator.
    assert(new_atom->string);

    // Copy the string into the new atom's memory.
    strncpy(new_atom->string, string, length);
    new_atom->string[length] = '\0';  // Ensure the string is null-terminated.

    // Insert the new atom at the beginning of the linked list.
    new_atom->link = buckets[h];
    buckets[h] = new_atom;
    
    return new_atom->string;
}

const char *Atom_string(const char *string) {
    assert(string);
    return Atom_new(string, strlen(string));
}

const char *Atom_int(const long n) {
    char *str = malloc(43);  // Allocate enough space for the string representation.
    assert(str);  // Check if allocation succeeded.
    sprintf(str, "%ld", n);
    const char *result = Atom_new(str, (int)strlen(str));
    free(str);  // Free the temporary string after use.
    return result;
}