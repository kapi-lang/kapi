#ifndef crypto_hash_h
#define crypto_hash_h

#include <stddef.h>
#include <stdint.h>

typedef uint32_t Hash;

/**
 * Using FNV-1a algorithm
 */
Hash hash(const char *value);

/**
 * Using FNV-1a algorithm
 */
Hash hash_with_length(const char *value, size_t length);

#endif
