#include <controlmanual/core/util.h>
#include <controlmanual/core/ui.h>
#include <controlmanual/core/tcontext.h>
#include <controlmanual/core/data.h>
#include <stdlib.h>
#include <stdio.h> // fprintf, stderr
#include <errno.h>
#include <string.h> // strerror
#include <math.h> // log10, ceil
#include <signal.h>
#include <stdnoreturn.h>
#define STR_OR_NULL(d) d ? CONTENT_STR(d) : "<null>"
#define INCBUF(amount) do { bufsize += amount; str = safe_realloc(str, bufsize); } while (0);
#define CATSTR(s) do { \
    INCBUF(strlen(s)); \
    strcat(str, strdup(s)); \
} while (0);
#define INTFMT(tp, fmt) { \
                    tp i = va_arg(vargs, tp); \
                    size_t size = (int) ceil(log10(i) + 2); \
                    char* tmp = safe_malloc(size); \
                    sprintf(tmp, "%" #fmt, i); \
                    break; \
                }


noreturn void fail(const char* message, int lineno, const char* file) {
#ifdef CM_DEBUG
    signal(SIGSEGV, SIG_DFL);
    /* we need to reset the signal handler in case something goes wrong in this function */
#endif
    fprintf(
        stderr,
        "(%s:%d) fatal control manual error: %s\n\n"
        "-- DEBUG INFORMATION --\n\n",
        /* 
            NOTE: lineno and file only show where fail
            was called, not where the problem occured.
        */
        file,
        lineno,
        message
    );

    if (errno)
        fprintf(
            stderr,
            "errno: %d\n"
            "strerror: %s\n\n",
            errno,
            strerror(errno)
        );

    if (cm_runtime_tcontext) {
        // tcontext is initalized
        fprintf(
            stderr,
            "-=- tcontext frame information -=-\n"
            "current name: %s\n"
            "origin: %s\n"
            "context available: %s\n"
            "state: %s\n"
            "-=- end tcontext frame information -=-\n",
            STR_OR_NULL(cm_runtime_tcontext->name),
            STR_OR_NULL(cm_runtime_tcontext->origin),
            cm_runtime_tcontext->ctx ? "yes" : "no",
            tcontext_state_name(cm_runtime_tcontext->state)
        );
    } else
        fputs(
            "runtime not initalized, no further information available\n",
            stderr
        );

    fputs("\n-- END DEBUG INFORMATION --\n", stderr);
    fflush(stderr);
    abort();
}

/* Allocate memory and handle NULL. */
void* safe_malloc(size_t nbytes) {
    void* m = malloc(nbytes);
    if (!m) FAIL("malloc() returned NULL!");
    return m;
}

/* Reallocate memory and handle NULL. */
void* safe_realloc(void* ptr, size_t nbytes) {
    void* m = realloc(ptr, nbytes);
    if (!m) FAIL("realloc() returned NULL!");
    return m;
}

/* Allocate an array and handle NULL. */
void* safe_calloc(size_t num, size_t size) {
    void* m = calloc(num, size);
    if (!m) FAIL("calloc() returned NULL!");
    return m;
}

/* Use an integer as a pointer by allocating it on the heap.*/
int* int_convert(int value) {
    int* i = safe_malloc(sizeof(int));
    *i = value;
    return i;
}

char* format_size_va(
    const char* fmt,
    size_t* bufsize_target,
    va_list vargs,
    ...
) {
    size_t len = strlen(fmt);
    size_t bufsize = 1;
    char* str = safe_malloc(1);
    strcpy(str, "");

    for (int i = 0; i < len; i++) {
        char c = fmt[i];
        char last = i != 0 ? fmt[i - 1] : '_';

        if (last == '%') {
            switch (c) {
                case 's': {
                    char* ptr = va_arg(vargs, char*);
                    CATSTR(ptr);
                    break;
                }

                case 'S': {
                    object* ob = va_arg(vargs, object*);
                    object* ob_str = object_to_string(ob);
                    if (!ob_str) return NULL;
                    char* result = STRING_VALUE(ob_str);
                    CATSTR(result);
                    break;
                }
                
                case 'i': INTFMT(int, d);
                case 'u': INTFMT(unsigned int, u);
                case 'U': INTFMT(unsigned long, lu);
                case 'l': INTFMT(long, ld);
                case 'L': INTFMT(long long, lld);
                case 'f': {
                    double f = va_arg(vargs, double);
                    int len = snprintf(NULL, 0, "%f", f);
                    char* result = safe_malloc(len + 1);
                    snprintf(result, len + 1, "%f", f);
                    CATSTR(result);
                    break;
                }

                case 'p': {
                    void* ptr = va_arg(vargs, void*);
                    int len = snprintf(NULL, 0, "%p", ptr);
                    char* result = safe_malloc(len + 1);
                    snprintf(result, len + 1, "%p", ptr);
                    CATSTR(result);
                    break;
                }

                case 'c': {
                    int ch = va_arg(vargs, int);
                    char* chstr = char_to_string((char) ch);
                    INCBUF(2);
                    strcat(str, chstr);
                    break;
                }

                case '%': {
                    char* tmp = char_to_string('%');
                    INCBUF(2);
                    strcat(str, tmp);
                    break;
                }
            }
        }
    }

    if (bufsize_target) *bufsize_target = bufsize;
    return str;
}

char* format_size(const char* fmt, size_t* bufsize, ...) {
    va_list vargs;
    va_start(vargs, bufsize);
    char* result = format_size_va(fmt, bufsize, vargs);
    va_end(vargs);
    return result;
}

inline char* format_va(const char* fmt, va_list vargs) {
    return format_size_va(fmt, NULL, vargs);
}

char* format(const char* fmt, ...) {
    va_list vargs;
    va_start(vargs, fmt);
    char* result = format_size_va(fmt, NULL, vargs);
    va_end(vargs);
    return result;
}

