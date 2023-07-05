#ifndef CM_UTIL_H
#define CM_UTIL_H

#include <stdlib.h>
#include <stdarg.h> // va_list
#include <stdnoreturn.h>
#define REALLY_UNPAREN(...) __VA_ARGS__
#define INVOKE(expr) expr
#define UNPAREN(args) INVOKE(REALLY_UNPAREN args)
#define FUNCTYPE(name, ret, args) ret (*name)(UNPAREN(args));
#define RETN(expr) { expr; return NULL; }

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && \
    !defined(__CYGWIN__)
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

    #if (defined(__GNUC__) && (__GNUC__ >= 4)) || \
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
#define FAIL(message) fail(message, __LINE__, __FILE__, __func__)
#else
#define FAIL(message) fail(message, 0, "<unknown>.c", __func__)
#endif

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#define PLATFORM_POSIX
#endif

#if !defined(PLATFORM_POSIX) && !defined(PLATFORM_WIN)
#error "this platform is not supported"
#endif

#ifdef __GNUC__
#define COMPILER_GNU
#endif

#ifdef __clang__
#define COMPILER_CLANG
#endif

#ifdef _MSC_VER
#define COMPILER_MSVC
#endif

#if !defined(COMPILER_GNU) && !defined(COMPILER_CLANG) && \
    !defined(COMPILER_MSVC)
#warning "this compiler is not gcc, clang, or msvc"
#endif

#if defined(COMPILER_GNU) || defined(COMPILER_CLANG)
#define COMPILER_GCC_LIKE
#endif

#ifdef COMPILER_GCC_LIKE
#define UNUSED __attribute__((unused))
#else
#define UNUSED
#endif

#ifdef CM_COMPILING
#define API EXPORT
#else
#define API IMPORT
#endif

API void* safe_malloc(size_t bytes);
API void* safe_realloc(void* ptr, size_t nbytes);
API void* safe_calloc(size_t num, size_t size);
API noreturn void fail(
    const char* message,
    int lineno,
    const char* file,
    const char* func_name
);

API int* int_convert(int value);
API char* format_size_va(
    const char* fmt,
    size_t* bufsize_target,
    va_list vargs,
    ...
);
API char* format_size(const char* fmt, size_t* bufsize, ...);
API char* format(const char* fmt, ...);
API extern char* format_va(const char* fmt, va_list vargs);
API char* safe_strdup(const char* data);

#endif
