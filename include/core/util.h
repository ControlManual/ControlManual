#ifndef CM_UTIL_H
#define CM_UTIL_H

#include <stdlib.h>
#define _REALLY_UNPAREN(...) __VA_ARGS__
#define _INVOKE(expr) expr
#define _UNPAREN(args) _INVOKE(_REALLY_UNPAREN args)
#define FUNCTYPE(name, ret, args) ret (*name)(_UNPAREN(args));

void error_no_memory();
void* safe_malloc(size_t bytes);
void* safe_realloc(void* ptr, size_t nbytes);
void* safe_calloc(size_t num, size_t size);
void fail(const char* message);

#endif
