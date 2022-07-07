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

/* Create a new map. */
map* map_new(void) {
    map* m = (map*) malloc(sizeof(map));
    if (!m) NOMEM("map_new");
    m->size = 0;
    m->items = calloc(0, sizeof(_kv));
    return m;
}

/* Get a value from a map. */
void* map_get(map* m, char* key) {
    NONULL(m, "map_get");

    for (int i = 0; i < m->size; i++) {
        if (!strcmp(m->items[i]->key, key)) {
            return m->items[i]->value;
        }
    }

    THROW("map_get", "key not found", 0);

    return NULL;
}

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

static node* _linked_list_alloc(void* value) {
    node* n = (node*) malloc(sizeof(node));
    if (!n) NOMEM("_linked_list_alloc");
    n->value = value;
    return n;
}

node* linked_list_new(void* value) {
    node* n = _linked_list_alloc(value);
    size_t* size = (size_t*) malloc(sizeof(size_t));
    if (!size) NOMEM("linked_list_new");

    n->index = 0;
    n->next = NULL;
    n->last = NULL;
    n->size = size;

    return n;
}

node* linked_list_append(node* list, void* value) {
    NONULL(list, "linked_list_append");
    node* n = _linked_list_alloc(value);
    *(list->size)++;
    n->index = list->index + 1;
    n->next = list->next;
    n->last = list;
    n->size = list->size;
    list->next = n;
    
    node* next = n->next;

    while (next) {
        next->index++;
        next = next->next;
    }

    return n;
}

void* linked_list_get(node* list, int index) {
    NONULL(list, "linked_list_get");
    node* current = list;
    unsigned char backwards = index < list->index;

    while (current) {
        if (index == current->index) {
            return current->value;
        }
        if (backwards) current = current->last;
        else current = current->next;
    }

    THROW("linked_list_get", "invalid index", 0);
    return NULL;
}

void linked_list_free(node* list) {
    NONULL(list, "linked_list_free");
    node* last = list->last;
    node* next = list->next;

    LINKEDLIST_FREE(last);
    LINKEDLIST_FREE(next);
    free(list);
}

void linked_list_remove(node* list, int index) {
    NONULL(list, "linked_list_remove");
    unsigned char backwards = index < list->index;

    while (next) {
        next->index++;
        next = next->next;
    }
}