#include <stdlib.h>
#include <stdio.h> // fprintf, stderr
#include <core/util.h>

void fail(const char* message, int lineno, const char* file) {
    fprintf(
        stderr,
        "(%s:%d) fatal control manual error: %s\n", 
        file,
        lineno,
        message
    );
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

int* int_convert(int value) {
    int* i = safe_malloc(sizeof(int));
    *i = value;
    return i;
}
