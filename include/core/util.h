#ifndef CM_UTIL_H
#define CM_UTIL_H

#include <stdlib.h>
#define REALLY_UNPAREN(...) __VA_ARGS__
#define INVOKE(expr) expr
#define UNPAREN(args) INVOKE(REALLY_UNPAREN args)
#define FUNCTYPE(name, ret, args) ret (*name)(UNPAREN(args));
#define RETN(expr) { expr; return NULL; }

#if defined(__LINE__) && defined(__FILE__)
#define FAIL(message) fail(message, __LINE__, __FILE__)
#else
#define FAIL(message) fail(message, 0, "<unknown>.c")
#endif

void* safe_malloc(size_t bytes);
void* safe_realloc(void* ptr, size_t nbytes);
void* safe_calloc(size_t num, size_t size);
void fail(const char* message, int lineno, const char* file);

int* int_convert(int value);

#endif
