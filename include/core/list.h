#ifndef CM_LIST_H
#define CM_LIST_H

#include <core/data.h>
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

list* list_new(void);
void list_append(list* l, data* value);
extern void* list_get(list* l, size_t index);
void list_remove(list* l, size_t index);
void list_free(list* l);

#endif