#include "file_utils.h"

#include <stdio.h>
#include <stdlib.h>

#include "code_errors.h"
#include "safe_alloc.h"

int get_size_file(long* size_file, const char* filename) {
  int rc = EXIT_SUCCESS;

  FILE* file = fopen(filename, "r");
  if (!file) {
    rc = FOPEN_ERROR;
  }
  if (!rc) {
    if (fseek(file, 0, SEEK_END)) {
      rc = FSEEK_ERROR;
    }
  }
  if (!rc) {
    *size_file = ftell(file);
    if (*size_file == -1) {
      rc = FTELL_ERROR;
    }
  }
  if (file) {
    rewind(file);
    fclose(file);
  }

  return rc;
}

int read_file_to_buffer(char** buf, const char* filename) {
  int rc = EXIT_SUCCESS;
  long size_file;

  FILE* file = fopen(filename, "r");
  if (!file) {
    rc = FOPEN_ERROR;
  }
  if (!rc) {
    rc = get_size_file(&size_file, filename);
  }
  if (!rc) {
    *buf = (char*)safe_alloc(NULL, size_file + 1, sizeof(char), &rc);
  }
  if (!rc) {
    size_t read_bytes = fread(*buf, 1, size_file, file);
    if (read_bytes != (size_t)size_file) {
      rc = FREAD_ERROR;
    }
  }
  if (!rc) {
    *((*buf) + size_file) = '\0';
  }
  if (file) {
    fclose(file);
  }

  return rc;
}