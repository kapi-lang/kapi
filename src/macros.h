#ifndef macros_h
#define macros_h

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// General

#define LOG_INFO(...) fprintf(stdout, __VA_ARGS__)
#if DEBUG
#define LOG_DEBUG(...) fprintf(stdout, __VA_ARGS__)
#else
#define LOG_DEBUG(...)
#endif
#define LOG_ERROR(...) fprintf(stderr, __VA_ARGS__)

#define EXPECT(condition, ...)                                                 \
  if (!(condition)) {                                                          \
    LOG_ERROR("\nAssertion Failed: ");                                         \
    LOG_ERROR(__VA_ARGS__);                                                    \
    LOG_ERROR("\n");                                                           \
    LOG_ERROR("\t%s", #condition);                                             \
    LOG_ERROR("\t%s:%d\n", __FILE__, __LINE__);                                \
    LOG_ERROR("\n");                                                           \
                                                                               \
    abort();                                                                   \
  }

#define EXCEPTION(...)                                                         \
  LOG_ERROR(__VA_ARGS__);                                                      \
  abort();

#define NEW(type) (type *)malloc(sizeof(type))

#define FOR_RANGE(start, end, op)                                              \
  for (size_t i = start; i < (end + 1); i++)                                   \
  op

#define FOR_I(length, op)                                                      \
  for (size_t i = 0; i < (length); i++)                                        \
  op

#define FOR_ENDLESS(op)                                                        \
  for (;;)                                                                     \
  op

#define PTR_TERNARY(pointer, then, else) ((pointer != NULL) ? (then) : (else))

#define TYPEDEF_FN(name, return, ...) typedef return (*name)(__VA_ARGS__);

// Number

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#define MAX(a, b) (((a) > (b)) ? (a) : (b))

// Array

#define ARRAY_NEW(type, capacity) (type *)malloc((capacity) * sizeof(type))

#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

#define ARRAY_RESIZE(type, array, capacity)                                    \
  (type *)realloc(array, (capacity) * sizeof(type))

#define ARRAY_PUSH(type, array, capacity, value)                               \
  {                                                                            \
    const size_t index = capacity++;                                           \
    array = ARRAY_RESIZE(type, array, capacity);                               \
    array[index] = value;                                                      \
  }

// String

#define STRING_NEW(length) (char *)malloc(sizeof(char) * (length + 1))

#define STRING_RESIZE(target, length)                                          \
  (char *)realloc(target, sizeof(char) * (length + 1))

#endif
