#ifndef CM_VECTOR_H
#define CM_VECTOR_H

#include <stdlib.h>
#include <controlmanual/core/data.h>
#include <controlmanual/core/util.h>

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

API vector* vector_new(void);
API void vector_free(vector* vec);
API void vector_append(vector* vec, data* item);
API void vector_insert(vector* vec, size_t index, data* item);
API data* vector_pop(vector* vec, size_t index);
API extern void vector_remove(vector* vec, size_t index);
API extern void* vector_get(vector* vec, size_t index);
API extern void vector_set(vector* vec, size_t index, data* value);
API data* vector_get_data(vector* vec, size_t index);
API vector* vector_copy(const vector* vec);
API extern data* vector_get_data_new(vector* vec, size_t index);

#endif