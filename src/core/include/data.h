#ifndef __CM_DATA_H__
#define __CM_DATA_H__
#include <stdbool.h>
#define HEAP_DATA(contents) data_new(true, contents)
#define STACK_DATA(contents) data_new(false, contents)

typedef struct STRUCT_DATA {
    void* contents;
    bool should_free;
} arbitrary_data;

arbitrary_data* data_new(bool should_free, void* contents);
void data_free(arbitrary_data* data);

#endif