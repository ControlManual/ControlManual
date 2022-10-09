#include <core/util.h> // safe_malloc
#include <core/data.h>
#include <stdbool.h>
#include <stdlib.h>

data* data_new(void* contents, bool should_free, data_dealloc dealloc) {
    data* d = safe_malloc(sizeof(data));
    size_t* size = safe_malloc(sizeof(size_t));
    *size = 1;
    d->contents = contents;
    d->should_free = should_free;
    d->dealloc = dealloc;
    d->refs = size;
    return d;
}

data* data_from(data* d) {
    (*d->refs)++;
    data* new_d = safe_malloc(sizeof(data));
    new_d->contents = d->contents;
    new_d->should_free = d->should_free;
    new_d->dealloc = d->dealloc;
    new_d->refs = d->refs;
    return new_d;
}

inline void data_free(data* d) {
    (*d->refs)--;
    if (!(*d->refs)) {
        if (d->should_free)
            (d->dealloc ? d->dealloc : free)(d->contents);
        
        free(d->refs);
    }

    free(d);
}

/* Get the data's contents. */
inline void* data_content(data* d) {
    return d->contents;
}

/* Get the data's contents, but a NULL argument is allowed. */
inline void* data_content_maybe(data* d) {
    if (!d) return NULL;
    return d->contents;
}
