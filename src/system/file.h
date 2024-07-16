#ifndef file_h
#define file_h

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

bool file_exists(const char *path);

FILE *file_open(const char *path);

bool file_close(FILE *file);

size_t file_write(FILE *file, const void *buffer, size_t size, size_t length);

char *file_read(const char *path);

bool file_rename(const char *path, const char *next_path);

#endif
