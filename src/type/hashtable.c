#include <pthread.h>
#include <stdlib.h>

#include "macros.h"
#include "system/hash.h"
#include "type/hashtable.h"
#include "type/string.h"

#define TABLE_MAX_LOAD 0.75

#define GROW_CAPACITY(capacity) ((capacity) < 8 ? 8 : (capacity) * 2)

typedef struct HashTableEntry {
  char *key;
  void *value;
} HashTableEntry;

struct HashTable {
  HashTableEntry *entries;
  size_t capacity;
  size_t length;
  pthread_mutex_t mutex;
};

HashTable *hashtable_create() {
  HashTable *table = NEW(HashTable);

  table->length = 0;
  table->capacity = 0;
  table->entries = NULL;

  pthread_mutex_init(&table->mutex, NULL);

  return table;
}

void hashtable_free(HashTable *table) {
  EXPECT(table != NULL, "[hash table] invalid table\n");

  pthread_mutex_lock(&table->mutex);
  {
    FOR_I(table->capacity, {
      string_free(table->entries[i].key);
      free(table->entries[i].value);
    });

    free(table->entries);
  }
  pthread_mutex_unlock(&table->mutex);

  pthread_mutex_destroy(&table->mutex);

  free(table);
}

size_t hashtable_size(HashTable *table) {
  EXPECT(table != NULL, "[hash table] invalid table\n");

  pthread_mutex_lock(&table->mutex);
  size_t size = table->length;
  pthread_mutex_unlock(&table->mutex);

  return size;
}

static void *hashtable_find_entry(HashTableEntry *entries, size_t capacity,
                                  const char *key) {
  size_t index = hash(key) & (capacity - 1);

  HashTableEntry *tombstone = NULL;

  FOR_ENDLESS({
    HashTableEntry *entry = &entries[index];

    if (entry->key == NULL) {
      if (entry->value == NULL) {
        return tombstone != NULL ? tombstone : entry;
      }

      if (tombstone == NULL) {
        tombstone = entry;
      }

      continue;
    }

    if (string_equal(entry->key, key)) {
      return entry;
    }

    index = (index + 1) & (capacity - 1);
  })

  return NULL;
}

void *hashtable_get(HashTable *table, const char *key) {
  EXPECT(table != NULL, "[hash table] invalid table\n");

  if (hashtable_size(table) == 0) {
    return NULL;
  }

  void *data = NULL;

  pthread_mutex_lock(&table->mutex);
  {
    HashTableEntry *entry =
        hashtable_find_entry(table->entries, table->capacity, key);

    if (entry != NULL && entry->key != NULL) {
      data = entry->value;
    }
  }
  pthread_mutex_unlock(&table->mutex);

  return data;
}

void hashtable_expand(HashTable *table, size_t capacity) {
  HashTableEntry *entries = ARRAY_NEW(HashTableEntry, capacity);

  EXPECT(entries != NULL, "[hash table] error to expand\n");

  for (size_t i = 0; i < capacity; i++) {
    entries[i].key = NULL;
    entries[i].value = NULL;
  }

  table->length = 0;

  for (size_t i = 0; i < table->capacity; i++) {
    const HashTableEntry *entry = &table->entries[i];

    if (entry->key == NULL) {
      continue;
    }

    HashTableEntry *dest = hashtable_find_entry(entries, capacity, entry->key);

    dest->key = entry->key;
    dest->value = entry->value;

    table->length++;
  }

  free(table->entries);

  table->entries = entries;
  table->capacity = capacity;
}

void hashtable_set(HashTable *table, const char *key, void *value) {
  EXPECT(table != NULL, "[hash table] invalid table\n");
  EXPECT(value != NULL, "[hash table] can't set a NULL value\n");

  pthread_mutex_lock(&table->mutex);

  if (table->length + 1 > table->capacity * TABLE_MAX_LOAD) {
    hashtable_expand(table, GROW_CAPACITY(table->capacity));
  }

  HashTableEntry *entry =
      hashtable_find_entry(table->entries, table->capacity, key);
  const bool is_new_entry = entry->key == NULL;

  if (is_new_entry && entry->value == NULL) {
    table->length++;
  }

  entry->key = string_from(key);
  entry->value = value;

  pthread_mutex_unlock(&table->mutex);
}

void hashtable_delete(HashTable *table, const char *key) {
  EXPECT(table != NULL, "[hash table] invalid table\n");
  EXPECT(hashtable_size(table) > 0, "[hash table] table is empty\n");

  pthread_mutex_lock(&table->mutex);
  {
    HashTableEntry *entry =
        hashtable_find_entry(table->entries, table->capacity, key);

    if (entry->key != NULL) {
      entry->key = NULL;
      entry->value = NULL;
    }
  }
  pthread_mutex_unlock(&table->mutex);
}
