#include <string.h>

#include "system/hash.h"

Hash hash(const char *value) { return hash_with_length(value, strlen(value)); }

Hash hash_with_length(const char *value, size_t length) {
  Hash hash = 2166136261u;

  for (size_t i = 0; i < length; i++) {
    hash ^= (uint8_t)value[i];
    hash *= 16777619;
  }

  return hash;
}
