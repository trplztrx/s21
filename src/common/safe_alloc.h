#ifndef SAFE_ALLOC_H
#define SAFE_ALLOC_H

#include <stdlib.h>

void* safe_alloc(void* ptr, size_t count, size_t size, int* rc);

#define FREE(ptr) ((void)(free(ptr), (ptr) = NULL))

#endif  // SAFE_ALLOC_H