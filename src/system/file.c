#include <unistd.h>

#include "macros.h"
#include "system/file.h"

bool file_exists(const char *path) { return access(path, F_OK) == 0; }

FILE *file_open(const char *path) { return fopen(path, "wb"); }

bool file_close(FILE *file) { return fclose(file) == 0; }

size_t file_write(FILE *file, const void *buffer, size_t size, size_t length) {
  return fwrite(buffer, size, length, file);
}

char *file_read(const char *path) {
  FILE *file = fopen(path, "rb");

  if (file == NULL) {
    LOG_ERROR("[file] could not open '%s'.\n", path);

    exit(74);
  }

  fseek(file, 0L, SEEK_END);

  const size_t fileSize = ftell(file);

  rewind(file);

  char *buffer = STRING_NEW(char, fileSize);

  if (buffer == NULL) {
    LOG_ERROR("[file] not enough memory to read '%s'.\n", path);

    exit(74);
  }

  const size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);

  if (bytesRead < fileSize) {
    LOG_ERROR("[file] could not read '%s'.\n", path);

    exit(74);
  }

  buffer[bytesRead] = '\0';

  fclose(file);

  return buffer;
}

bool file_rename(const char *path, const char *next_path) {
  return rename(path, next_path) == 0;
}
