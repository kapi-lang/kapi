#include <ctype.h>
#include <stdarg.h>
#include <string.h>

#include "macros.h"
#include "type/string.h"

static char *string_from_sized(const char *value, size_t length) {
  char *result = STRING_NEW(length);

  (void)memcpy(result, value, length);

  result[length] = '\0';

  return result;
}

char *string_empty() {
  char *result = STRING_NEW(1);

  result[0] = '\0';

  return result;
}

char *string_from(const char *value) {
  return string_from_sized(value, strlen(value));
}

void string_free(char *value) { free(value); }

void string_update(char *string, const char *value) {
  const size_t length = strlen(string);
  const size_t next_length = strlen(value);

  if (next_length > length) {
    string = STRING_RESIZE(string, next_length);
  }

  (void)memcpy(string, value, next_length);
  string[length] = '\0';
}

bool string_equal(const char *left, const char *right) {
  const size_t left_size = strlen(left);
  const size_t right_size = strlen(right);

  if (left_size != right_size) {
    return false;
  }

  return memcmp(left, right, left_size) == 0;
}

bool string_starts_with(const char *target, const char *prefix) {
  const size_t target_size = strlen(target);
  const size_t prefix_size = strlen(prefix);

  bool result = false;

  if (prefix_size <= target_size) {
    char *part_string = string_from_sized(target, prefix_size);

    result = string_equal(prefix, part_string);

    string_free(part_string);
  }

  return result;
}

bool string_ends_with(const char *target, const char *suffix) {
  const size_t target_size = strlen(target);
  const size_t suffix_size = strlen(suffix);

  bool result = false;

  if (suffix_size <= target_size) {
    char *part_string =
        string_from_sized(target + target_size - suffix_size, suffix_size);

    result = string_equal(suffix, part_string);

    string_free(part_string);
  }

  return result;
}

/* -1 when the character is not found */
int string_index_of(const char *target, char find) {
  const size_t target_size = strlen(target);

  size_t i = 0;

  while (i < target_size && target[i] != find) {
    i += 1;
  }

  if (i < target_size) {
    return i;
  }

  return -1;
}

void string_trim(char **target) {
  string_trim_left(target);
  string_trim_right(target);
}

void string_trim_left(char **target) {
  const size_t current_size = strlen(*target);

  size_t i = 0;

  while (i < current_size && isspace((*target)[i])) {
    i += 1;
  }

  const size_t next_size = current_size - i;

  char *tmp = string_from_sized((*target) + i, next_size);

  *target = STRING_RESIZE(*target, next_size);

  (void)memcpy(*target, tmp, next_size);

  (*target)[next_size] = '\0';

  string_free(tmp);
}

void string_trim_right(char **target) {
  const size_t current_size = strlen(*target);

  size_t i = 0;

  while (i < current_size && isspace((*target)[current_size - 1 - i])) {
    i += 1;
  }

  const size_t next_size = current_size - i;

  char *tmp = string_from_sized(*target, next_size);

  *target = STRING_RESIZE(*target, next_size);

  (void)memcpy(*target, tmp, next_size);

  (*target)[next_size] = '\0';

  string_free(tmp);
}

void string_concat(char **target, const char *value) {
  const size_t current_size = strlen(*target);
  const size_t value_size = strlen(value);
  const size_t next_size = current_size + value_size;

  *target = STRING_RESIZE(*target, next_size);

  (void)memcpy((*target) + current_size, value, value_size);

  (*target)[next_size] = '\0';
}

char *string_remove_all(const char *value, const char remove) {
  char *next_value = string_empty();
  char *cursor = (char *)value;

  int i = string_index_of(value, remove);

  while (i != -1) {
    char *tmp = string_from_sized(cursor, i);
    string_concat(&next_value, tmp);
    string_free(tmp);

    cursor = cursor + (i + 1);
    i = string_index_of(cursor, remove);
  }

  if (cursor[0] != '\0') {
    char *tmp = string_from_sized(cursor, strlen(cursor));
    string_concat(&next_value, tmp);
    string_free(tmp);
  }

  return next_value;
}

char *string_format(const char *format, ...) {
  va_list args;
  va_start(args, format);
  size_t length = vsnprintf(0, 0, format, args);
  va_end(args);

  char *output = STRING_NEW(length);

  va_start(args, format);
  vsnprintf(output, length + 1, format, args);
  va_end(args);

  return output;
}
