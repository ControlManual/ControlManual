/* NOTE: this a temporary implementation, it doesn't have any hashing as of now. */

#include <core/map.h>
#include <core/error.h>
#include <string.h>
#include <stdio.h>

map* map_new(void) {
    map* m = safe_malloc(sizeof(map));
    m->items = vector_new();
    m->size = 0;

    return m;
}

pair* map_get_pair(map* m, const char* key, int* index) {
    for (int i = 0; i < m->size; i++) {
        if (!strcmp(
            ((pair*) vector_get(m->items, i))->key->contents,
            key
        )) {
            if (index) *index = i;
            return m->items->items[i]->contents;
        }
    }

    if (index) *index = -1;
}

inline void* map_get(map* m, const char* key) {
    pair* p = map_get_pair(m, key, NULL);
    if (!p) return NULL;

    return p->value->contents;
}

void map_remove(map* m, const char* key) {
    int index;
    map_get_pair(m, key, &index);
    vector_pop(m->items, index);
}

static inline void pair_dealloc(pair* p) {
    data_free(p->key);
    data_free(p->value);
    free(p);
}

void map_add(map* m, data* key, data* value) {
    pair* p = safe_malloc(sizeof(pair));
    p->key = key;
    p->value = value;
    
    int index;
    data* d = CUSTOM_DATA(p, &pair_dealloc);

    map_get_pair(m, key->contents, &index);
    if (catch_err()) vector_append(m->items, d);
    else vector_set(m->items, index, d);
    m->size++;
}

inline void map_free(map* m) {
    vector_free(m->items);
    free(m);
}