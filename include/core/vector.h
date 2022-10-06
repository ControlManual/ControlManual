#ifndef CM_VECTOR_H
#define CM_VECTOR_H

#include <stdlib.h>
#include "data.h"

/* Dynamic array that may hold any type. */
typedef struct STRUCT_VECTOR {
    size_t size;
    data** items;
} vector;

vector* vector_new(void);
void vector_free(vector* vec);
void vector_append(vector* vec, data* item);
void vector_insert(vector* vec, size_t index, data* item);
data* vector_pop(vector* vec, size_t index);
extern void vector_remove(vector* vec, size_t index);
extern void* vector_get(vector* vec, size_t index);
extern data* vector_get_data(vector* vec, size_t index);
extern void vector_set(vector* vec, size_t index, data* value);

#endif