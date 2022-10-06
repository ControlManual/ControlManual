#ifndef CM_MAP_H
#define CM_MAP_H

#include <stdlib.h>
#include "data.h"
#include "vector.h"

typedef struct STRUCT_PAIR {
	data* key;
	data* value;
} pair;


typedef struct STRUCT_MAP {
	size_t size;
	vector* items;
} map;

map* map_new(void);
void map_add(map* m, data* key, data* value);
void map_remove(map* m, const char* key);
pair* map_get_pair(map* m, const char* key, int* index);
void map_free(map* m);

extern void* map_get(map* m, const char* key);
extern void map_free(map* m);

#endif