#ifndef CM_DATA_H
#define CM_DATA_H

#include <stdbool.h>
#include <stdlib.h> // size_t
#include <core/util.h> // safe_malloc

#define HEAP_DATA(d) data_new(d, true, NULL)
#define CUSTOM_DATA(d, dealloc) data_new(d, true, (data_dealloc) dealloc)
#define STACK_DATA(d) data_new(d, false, NULL)
// Same as STACK_DATA, but saying its on the stack can be misleading
#define NOFREE_DATA(d) data_new(d, false, NULL)



typedef void (*data_dealloc)(void*);

/* Data that can have its contents freed accordingly. This should always be stored on the heap. */
typedef struct STRUCT_DATA {
    bool should_free;
    void* contents;
    data_dealloc dealloc;
    size_t* refs;
} data;

data* data_new(void* contents, bool should_free, data_dealloc dealloc);
extern void data_free(data* d);
data* data_from(data* d);
extern void* data_content(data* d);
extern void* data_content_maybe(data* d);

#endif
