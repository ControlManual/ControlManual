#ifndef CM_DATA_H
#define CM_DATA_H

#include <stdbool.h>
#include <stdlib.h> // size_t
#include <controlmanual/core/util.h> // safe_malloc

#define HEAP_DATA(d) data_new(d, true, NULL)
#define CUSTOM_DATA(d, dealloc) data_new(d, true, (data_dealloc) dealloc)
#define STACK_DATA(d) data_new(d, false, NULL)
// Same as STACK_DATA, but sometimes saying something is on the stack can be misleading
#define NOFREE_DATA(d) data_new(d, false, NULL)
#define DATA_FREE_MAYBE(d) { if (d) data_free(d); }

typedef void (*data_dealloc)(void*);

/* A reference to arbitrary data that may be stored on the stack or heap. */
typedef struct STRUCT_DATA data;

data* data_new(void* contents, bool should_free, data_dealloc dealloc);
extern void data_free(data* d);
data* data_from(data* d);
extern void* data_content(data* d);
extern void* data_content_maybe(data* d);
extern size_t data_sizeof(void);

extern void data_inc(data* d);
extern void data_dec(data* d);

#endif
