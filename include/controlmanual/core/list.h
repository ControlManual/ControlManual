#ifndef CM_LIST_H
#define CM_LIST_H

#include <controlmanual/core/data.h>
#include <controlmanual/core/util.h>
#include <stdlib.h> // size_t

typedef struct STRUCT_LIST_NODE list_node;

typedef struct STRUCT_LIST_NODE {
    list_node* next;
    list_node* last;
    data* value;
} list_node;

typedef struct STRUCT_LIST {
    list_node* first;
    list_node* last;
    size_t len;
} list;

API list* list_new(void);
API void list_append(list* l, data* value);
API extern void* list_get(list* l, size_t index);
API void list_remove(list* l, size_t index);
API void list_free(list* l);

#endif