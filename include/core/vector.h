#ifndef CM_VECTOR_H
#define CM_VECTOR_H

#include <stdlib.h>
#include <core/data.h>

/* Dynamic array that may hold any type. */
typedef struct STRUCT_VECTOR {
    size_t size;
    data** items;
} vector;

#define VECTOR_LENGTH(v) v->size

vector* vector_new(void);
void vector_free(vector* vec);
void vector_append(vector* vec, data* item);
void vector_insert(vector* vec, size_t index, data* item);
data* vector_pop(vector* vec, size_t index);
extern void vector_remove(vector* vec, size_t index);
extern void* vector_get(vector* vec, size_t index);
extern void vector_set(vector* vec, size_t index, data* value);
data* vector_get_data(vector* vec, size_t index);
vector* vector_copy(const vector* vec);

#endif