#include <stdlib.h>
#include <stdio.h> // printf

void fail(const char* message) {
    printf("ERROR: %s\n", message);
    abort();
}

void error_no_memory(void) {
    fail("Out of memory!");
}


/* Allocate memory and handle NULL. */
void* safe_malloc(size_t nbytes) {
    void* m = malloc(nbytes);
    if (!m) error_no_memory();
    return m;
}

/* Reallocate memory and handle NULL. */
void* safe_realloc(void* ptr, size_t nbytes) {
    void* m = realloc(ptr, nbytes);
    if (!m) error_no_memory();
    return m;
}

/* Allocate an array and handle NULL. */
void* safe_calloc(size_t num, size_t size) {
    void* m = calloc(num, size);
    if (!m) error_no_memory();
    return m;
}