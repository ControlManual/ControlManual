#ifndef CM_DATA_H
#define CM_DATA_H

#include <stdbool.h>

#define HEAP_DATA(d) data_new(d, true, NULL)
#define CUSTOM_DATA(d, dealloc) data_new(d, true, (data_dealloc) dealloc)
#define STACK_DATA(d) data_new(d, false, NULL)

typedef void (*data_dealloc)(void*);

/* Data that can have its contents freed accordingly. This itself should never be on the stack. */
typedef struct STRUCT_DATA {
    bool should_free;
    void* contents;
    data_dealloc dealloc;
} data;

data* data_new(void* contents, bool should_free, data_dealloc dealloc);
extern void data_free(data* d);

#endif