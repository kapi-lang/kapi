#ifndef type_hashtable_h
#define type_hashtable_h

#include <stddef.h>

typedef struct HashTable HashTable;

HashTable *hashtable_create();

void hashtable_free(HashTable *table);

size_t hashtable_size(HashTable *table);

void *hashtable_get(HashTable *table, const char *key);

void hashtable_set(HashTable *table, const char *key, void *value);

void hashtable_delete(HashTable *table, const char *key);

#endif
