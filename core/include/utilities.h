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

typedef struct STRUCT_MAP_KV {
    char* key;
    void* value;
} _kv;

typedef struct STRUCT_MAP {
    _kv** items;
    size_t size;
} map;

map* map_new(void);
void* map_get(map* m, char* key);
void map_add(map* m, char* key, void* value);
void map_keys_free(map* m);
void map_values_free(map* m);
void map_full_free(map* m);
void map_free(map* m);

typedef struct STRUCT_NODE node;

struct STRUCT_NODE {
    void* value;
    int index;
    node* next;
    node* last;
    size_t* size;
};

node* linked_list_new(void* value);
node* linked_list_append(node* list, void* value);
void* linked_list_get(node* list, int index);
void linked_list_free(node* list);

#endif