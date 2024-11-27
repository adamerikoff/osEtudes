#ifndef ATOM_INCLUDED
#define ATOM_INCLUDED

extern int Atom_length(const char *string);
extern const char *Atom_new(const char *string, int length);
extern const char *Atom_string(const char *string);
extern const char *Atom_int(const long n);
extern unsigned long hash_string(const char *string);

#endif