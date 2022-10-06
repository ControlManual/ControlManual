#include <core/data.h>
#include <stdbool.h>
#include <core/error.h>
#include <stdlib.h>

data* data_new(void* contents, bool should_free, data_dealloc dealloc) {
    data* d = safe_malloc(sizeof(data));
    d->contents = contents;
    d->should_free = should_free;
    d->dealloc = dealloc;
    return d;
}

inline void data_free(data* d) {
    if (d->should_free) 
        (d->dealloc ? d->dealloc : free)(d->contents);
    free(d);
}