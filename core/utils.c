#include <utilities.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <exception.h>
#define MAP_ITEMS_FREE free(m->items); free(m);
#define LINKEDLIST_FREE(name) while (name) { node* tmp = name->name; free(name); name = tmp; }

/* Create a new vector. */
vector* vector_new(void) {
    vector* vec = (vector*) malloc(sizeof(vector));
    if (!vec) NOMEM("vector_new");
    
    vec->size = 0;
    vec->items = calloc(0, 0);
    return vec;
}

/* Free a vector. */
void vector_free(vector* vec) {
    NONULL(vec, "vector_free");
    free(vec->items);
    free(vec);
}

/* Free a vector, along with all of its items. */
void vector_full_free(vector* vec) {
    NONULL(vec, "vector_full_free");
    for (int i = 0; i < vec->size; i++)
        free(vec->items[i]);

    vector_free(vec);
}

/* Append a value to the target vector. */
void vector_append(vector* vec, void* item) {
    NONULL(vec, "vector_append");
    vec->size++;
    vec->items = realloc(vec->items, vec->size * sizeof(item));
    if (!vec->items) NOMEM("vector_append");
    vec->items[vec->size - 1] = item;
};

/* Insert a value at the target index. */
void vector_insert(vector* vec, int index, void* item) {
    NONULL(vec, "vector_insert");

    if (index > vec->size) FATAL("vector_insert", "index is above vector size");

    vec->size++;
    vec->items = realloc(vec->items, vec->size * sizeof(item));
    for (int i = vec->size - 1; i >= index; i--)
        if (i == index) vec->items[i] = item;
        else vec->items[i] = vec->items[i - 1];
    
};

/* Remove the index from the target vector and return its value. */
void* vector_pop(vector* vec, int index) {
    NONULL(vec, "vector_pop");
    void* item = vec->items[index];
    void** new_items = calloc(vec->size - 1, sizeof(void*));
    unsigned char offset = 0;

    for (int i = 0; i < vec->size; i++) 
        if (i != index) new_items[i - offset] = vec->items[i];
        else offset++;

    free(vec->items);
    vec->items = new_items;
    vec->size--;

    return item;
}

/* Remove the inedx from the target vector and free its value. */
inline void vector_remove(vector* vec, int index) {
    free(vector_pop(vec, index));
}

/* Create a new map. */
map* map_new(void) {
    map* m = (map*) malloc(sizeof(map));
    if (!m) NOMEM("map_new");
    m->size = 0;
    m->items = calloc(0, sizeof(_kv));
    return m;
}

/* Get a key-value pair from a map. */
_kv* map_get_node(map* m, char* key) {
    NONULL(m, "map_get_node");

    for (int i = 0; i < m->size; i++) 
        if (!strcmp(m->items[i]->key, key)) 
            return m->items[i];

    THROW("map_get_node", "key not found", 0);

    return NULL;
}

/* Get a value from a map. */
inline void* map_get(map* m, char* key) {
    return map_get_node(m, key);
}

/* Allocate a new key-value pair. */
static _kv* _kv_new(char* key, void* value) {
    _kv* kv = (_kv*) malloc(sizeof(_kv));
    if (!kv) NOMEM("_kv_new");

    kv->key = key;
    kv->value = value;

    return kv;
}

/* Add a value to the map. */
void map_add(map* m, char* key, void* value) {
    NONULL(m, "map_add");
    m->size++;
    m->items = realloc(m->items, m->size * sizeof(_kv));
    m->items[m->size - 1] = _kv_new(key, value);    
}

/* Free a map, along with its keys. */
void map_free_keys(map* m) {
    for (int i = 0; i < m->size; i++) {
        free(m->items[i]->key);
        free(m->items[i]);
    }

    MAP_ITEMS_FREE;
}

/* Free a map, along with its values. */
void map_free_values(map* m) {
    for (int i = 0; i < m->size; i++) {
        free(m->items[i]->value);
        free(m->items[i]);
    }

    MAP_ITEMS_FREE;
}

/* Free a map, along with its keys and values. */
void map_full_free(map* m) {
    NONULL(m, "map_full_free");

    for (int i = 0; i < m->size; i++) {
        free(m->items[i]->key);
        free(m->items[i]->value);
        free(m->items[i]);
    }


    MAP_ITEMS_FREE;
}

/* Free a map. */
void map_free(map* m) {
    NONULL(m, "map_free");

    for (int i = 0; i < m->size; i++) {
        free(m->items[i]);
    }

    MAP_ITEMS_FREE;
}

/* Internal function for allocating a linked list node. */
static node* _linked_list_alloc(void* value) {
    node* n = (node*) malloc(sizeof(node));
    if (!n) NOMEM("_linked_list_alloc");
    n->value = value;
    return n;
}

/* Create a new linked list. */
node* linked_list_new(void* value) {
    node* n = _linked_list_alloc(value);
    size_t* size = (size_t*) malloc(sizeof(size_t));
    if (!size) NOMEM("linked_list_new");

    n->next = NULL;
    n->last = NULL;
    n->size = size;

    return n;
}

/* Append to the current node a linked list. */
node* linked_list_append(node* list, void* value) {
    NONULL(list, "linked_list_append");
    node* n = _linked_list_alloc(value);
    *(list->size)++;
    n->next = list->next;
    n->last = list;
    n->size = list->size;
    list->next = n;

    return n;
}

/* Get the node at the offset from the current node. */
node* linked_list_get_node(node* list, int index) {
    NONULL(list, "linked_list_get_node");
    node* current = list;

    for (int i = 0; i < index; i++) {
        current = list->next;
    }

    if (!current) THROW("linked_list_get_node", "invalid index", 0);
    return current;
}

/* Get the value at the offset from the current node. */
inline void* linked_list_get(node* list, int index) {
    return linked_list_get_node(list, index)->value;
}

/* Free a linked list. */
void linked_list_free(node* list) {
    NONULL(list, "linked_list_free");
    node* last = list->last;
    node* next = list->next;

    LINKEDLIST_FREE(last);
    LINKEDLIST_FREE(next);
    free(list);
}

/* Remove the target node from the linked list and return it. */
node* linked_list_pop(node* n) {
    NONULL(n, "linked_list_pop");

    *(n->size)--;

    n->last->next = n->next;
    return n;
}

/* Remove the target from the linked list and free it. */
inline void linked_list_remove(node* n) {
    free(linked_list_pop(n));
}