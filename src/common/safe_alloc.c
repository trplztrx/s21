#include "safe_alloc.h"

#include "code_errors.h"

void* safe_alloc(void* ptr, size_t count, size_t size, int* rc) {
  if (count == 0 || size == 0) {
    count = size = 1;
  }

  void* new_ptr = realloc(ptr, count * size);
  if (!new_ptr) {
    *rc = ALLOC_ERROR;
  }

  return new_ptr;
}