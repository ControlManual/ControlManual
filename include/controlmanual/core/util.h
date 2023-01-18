#ifndef CM_UTIL_H
#define CM_UTIL_H

#include <stdlib.h>
#define REALLY_UNPAREN(...) __VA_ARGS__
#define INVOKE(expr) expr
#define UNPAREN(args) INVOKE(REALLY_UNPAREN args)
#define FUNCTYPE(name, ret, args) ret (*name)(UNPAREN(args));
#define RETN(expr) { expr; return NULL; }

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#define PLATFORM_WIN
#endif

// taken from cpython source

#ifdef PLATFORM_WIN
    #define IMPORT __declspec(dllimport)
    #define EXPORT __declspec(dllexport)
    #define LOCAL
#else
    #ifndef __has_attribute
    #define __has_attribute(x) 0  // Compatibility with non-clang compilers.
    #endif

    #if (defined(__GNUC__) && (__GNUC__ >= 4)) ||\
    (defined(__clang__) && __has_attribute(visibility))
        #define IMPORT __attribute__ ((visibility ("default")))
        #define EXPORT __attribute__ ((visibility ("default")))
        #define LOCAL __attribute__ ((visibility ("hidden")))
    #else
        #define IMPORT
        #define EXPORT
        #define LOCAL
    #endif
#endif

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
