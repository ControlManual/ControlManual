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
/* Unsafe way for getting a vector item. Only use if you are certain that the index is valid. */
#define VECTOR_GET(v, i) data_content(v->items[i])
/* Unsafe way for getting a vector item data object. Only use if you are certain that the index is valid. */
#define VECTOR_GET_DATA(v, i) v->items[i]

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
extern data* vector_get_data_new(vector* vec, size_t index);

#endif