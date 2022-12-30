#ifndef CM_MAP_H
#define CM_MAP_H

#include <controlmanual/core/data.h>
#include <stdlib.h>

typedef struct STRUCT_MAP_PAIR {
    data* key;
    data* value;
} pair;

typedef struct STRUCT_MAP {
    size_t len;
    size_t capacity;
    pair** items;
} map;

extern void* map_get(map* m, const char* key);
map* map_new(size_t inital_capacity);
void map_set(map* m, data* key, data* value);
void map_free(map* m);
map* map_copy(map* m);
data* map_get_data(map* m, const char* key);

#endif
