#include <utilities.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <exception.h>
#define MAP_ITEMS_FREE free(m->items); free(m);

vector* vector_new(void) {
    vector* vec = (vector*) malloc(sizeof(vector));
    if (!vec) NOMEM("vector_new");
    
    vec->size = 0;
    vec->items = calloc(0, 0);
    return vec;
}

void vector_free(vector* vec) {
    NONULL(vec, "vector_free");
    free(vec->items);
    free(vec);
}

void vector_full_free(vector* vec) {
    NONULL(vec, "vector_full_free");
    for (int i = 0; i < vec->size; i++)
        free(vec->items[i]);

    vector_free(vec);
}

void vector_append(vector* vec, void* item) {
    NONULL(vec, "vector_append");
    vec->size++;
    vec->items = realloc(vec->items, vec->size * sizeof(item));
    if (!vec->items) NOMEM("vector_append");
    vec->items[vec->size - 1] = item;
};

map* map_new(void) {
    map* m = (map*) malloc(sizeof(map));
    if (!m) NOMEM("map_new");
    m->size = 0;
    m->items = calloc(0, 0);
    return m;
}

void* map_get(map* m, const char* key) {
    NONULL(m, "map_get");
    for (int i = 0; i < m->size; i++) {
        if (!strcmp(m->items[i][0], key)) {
            return m->items[i][1];
        }
    }

    return NULL;
}

void map_add(map* m, char* key, void* value) {
    NONULL(m, "map_add");
    m->size++;
    m->items = realloc(m->items, m->size * sizeof(void**));
    void*** items = &m->items[m->size - 1];
    *items = calloc(1, sizeof(value));
    (*items)[0] = key;
    (*items)[1] = value;
}

void map_full_free(map* m) {
    NONULL(m, "map_full_free");
    for (int i = 0; i < m->size; i++) {
        free(m->items[i][0]);
        free(m->items[i][1]);
        free(m->items[i]);
    }

    MAP_ITEMS_FREE;
}

void map_free(map* m) {
    NONULL(m, "map_free");
    for (int i = 0; i < m->size; i++) {
        free(m->items[i]);
    }

    MAP_ITEMS_FREE;
}