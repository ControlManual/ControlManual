#ifndef __CM_UTILS_H__
#define __CM_UTILS_H__

#include <stdlib.h>
#include <exception.h>

typedef struct STRUCT_VECTOR {
    size_t size;
    void** items;
} vector;

vector* vector_new(void);
void vector_free(vector* vec);
void vector_full_free(vector* vec);
void vector_append(vector* vec, void* item);

typedef struct STRUCT_MAP {
    void*** items;
    size_t size;
} map;

map* map_new(void);
void* map_get(map* m, const char* key);
void map_add(map* m, char* key, void* value);
void map_full_free(map* m);
void map_free(map* m);

#endif