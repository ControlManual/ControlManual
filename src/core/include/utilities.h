#ifndef __CM_UTILS_H__
#define __CM_UTILS_H__

#include <stdlib.h>
#include <exception.h>

/* Structure holding vector data. */
typedef struct STRUCT_VECTOR {
    size_t size;
    void** items;
} vector;

vector* vector_new(void);
void vector_free(vector* vec);
void vector_full_free(vector* vec);
void vector_append(vector* vec, void* item);
void vector_insert(vector* vec, int index, void* item);
void* vector_pop(vector* vec, int index);
extern void vector_remove(vector* vec, int index);

typedef struct STRUCT_MAP_KV {
    char* key;
    void* value;
} _kv;

typedef struct STRUCT_MAP {
    _kv** items;
    size_t size;
} map;

map* map_new(void);
_kv* map_get_node(map* m, const char* key);
extern void* map_get(map* m, const char* key);
void map_add(map* m, char* key, void* value);
void map_keys_free(map* m);
void map_values_free(map* m);
void map_full_free(map* m);
void map_free(map* m);

typedef struct STRUCT_NODE node;

/* Structure containing data for a node inside a linked list. */
struct STRUCT_NODE {
    void* value;
    node* next;
    node* last;
    size_t* size;
    node** first;
};

node* linked_list_new(void* value);
node* linked_list_append(node* list, void* value);
node* linked_list_get_node(node* list, int index);
extern void* linked_list_get(node* list, int index);
void linked_list_free(node* list);
node* linked_list_pop(node* n);
extern void linked_list_remove(node* n);

#endif