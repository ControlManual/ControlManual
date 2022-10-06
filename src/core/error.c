#include <stdlib.h>
#include <stdio.h>
#include <core/data.h>

void error_no_memory(void) {
    puts("Out of memory!");
    abort();
}

void fail(const char* message) {
    printf("ERROR: %s\n", message);
    abort();
}

/* Allocate memory and handle NULL. */
inline void* safe_malloc(size_t bytes) {
    void* m = malloc(bytes);
    if (!m) error_no_memory();
    return m;
}

/* Reallocate memory and handle NULL. */
inline void* safe_realloc(void* ptr, size_t nbytes) {
    void* m = realloc(ptr, nbytes);
    if (!m) error_no_memory();
    return m;
}

/* Allocate an array and handle NULL. */
inline void* safe_calloc(size_t num, size_t size) {
    void* m = calloc(num, size);
    if (!m) error_no_memory();
    return m;
}