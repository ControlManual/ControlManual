#ifndef CM_ALLOC_H
#define CM_ALLOC_H

#include <stdlib.h>

void error_no_memory();
extern void* safe_malloc(size_t bytes);
extern void* safe_realloc(void* ptr, size_t nbytes);
extern void* safe_calloc(size_t num, size_t size);
void fail(const char* message);

#endif