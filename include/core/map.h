#ifndef CM_MAP_H
#define CM_MAP_H

#include <core/data.h>
#include <stdlib.h>

typedef struct STRUCT_MAP map;

extern void* map_get(map* m, const char* key);
map* map_new(size_t inital_capacity);
void map_set(map* m, data* key, data* value);
void map_free(map* m);
map* map_copy(map* m);
data* map_get_data(map* m, const char* key);

#endif
