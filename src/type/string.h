#ifndef type_string_h
#define type_string_h

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

char *string_empty();

char *string_from(const char *value);

void string_free(char *value);

void string_update(char *string, const char *value);

bool string_equal(const char *left, const char *right);

bool string_starts_with(const char *value, const char *prefix);

bool string_ends_with(const char *value, const char *suffix);

/**
 * -1 when the character not found
 */
int string_index_of(const char *value, char find);

void string_trim(char **value);

void string_trim_left(char **value);

void string_trim_right(char **value);

void string_concat(char **left, const char *right);

char *string_remove_all(const char *value, const char remove);

char *string_format(const char *, ...) __attribute__((format(printf, 1, 2)));

#endif
