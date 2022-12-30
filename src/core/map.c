#include <controlmanual/core/data.h>
#include <controlmanual/core/map.h>
#include <controlmanual/core/util.h> // safe_malloc
#include <stdlib.h> // size_t
#include <stdint.h> // uint64_t
#include <string.h> // strdup
#include <stdio.h>

#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

// https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function
static uint64_t hash_key(const char* key) {
    uint64_t hash = FNV_OFFSET;
    for (const char* p = key; *p; p++) {
        hash ^= (uint64_t) (unsigned char) (*p);
        hash *= FNV_PRIME;
    }
    return hash;
}


data* map_get_data(map* m, const char* key) {
    uint64_t hash = hash_key(key);
    size_t index = (size_t) (hash & (uint64_t) (m->capacity - 1));

    while (m->items[index] != NULL) {
        if (!strcmp(key, data_content(m->items[index]->key)))
            return m->items[index]->value;
        index++;
        if (index == m->capacity) {
            index = 0;
            // need to wrap around the table
        }
    }
    return NULL;
}

inline void* map_get(map* m, const char* key) {
    return data_content_maybe(map_get_data(m, key));
}

map* map_new(size_t inital_capacity) {
    map* m = safe_malloc(sizeof(map));
    m->len = 0;
    m->capacity = inital_capacity;
    m->items = safe_calloc(inital_capacity, sizeof(pair));
    return m;
}

static void set_entry(
    pair** items,
    size_t capacity,
    data* key,
    data* value,
    size_t* len
) {
    uint64_t hash = hash_key(data_content(key));
    size_t index = (size_t) (hash & (uint64_t) (capacity - 1));

    while (items[index] != NULL) {
        if (!strcmp(data_content(key), data_content(items[index]->key))) {
            data_free(items[index]->value);
            items[index]->value = value;
            data_free(key);
            return;
        }

        index++;
        if (index == capacity)
            index = 0;
    }

    if (len != NULL)
        (*len)++;

    if (!items[index])
        items[index] = safe_malloc(sizeof(pair));
    
    items[index]->key = key;
    items[index]->value = value;
}

static void expand(map* m) {
    size_t new_capacity = m->capacity * 2;
    if (new_capacity < m->capacity)
        FAIL("Integer limit reached on map capacity");
    pair** items = safe_calloc(new_capacity, sizeof(pair));

    for (int i = 0; i < m->capacity; i++) {
        pair* item = m->items[i];
        if (item) {
            set_entry(
                items,
                new_capacity,
                item->key,
                item->value,
                NULL
            );
            free(item);
        }
        
    }

    free(m->items);
    m->items = items;
    m->capacity = new_capacity;
}

void map_free(map* m) {
    for (int i = 0; i < m->capacity; i++) {
        pair* item = m->items[i];
        if (item) {
            data_free(item->key);
            data_free(item->value);
            free(item);
        }
    }
    
    free(m->items);
    free(m);
}

void map_set(map* m, data* key, data* value) {
    if (m->len >= m->capacity / 2)
        expand(m);
    set_entry(
        m->items,
        m->capacity,
        key,
        value,
        &m->len
    );
}

map* map_copy(map* m) {
    map* new = map_new(m->capacity);
    for (int i = 0; i < m->capacity; i++) {
        pair* item = m->items[i];
        if (item)
            map_set(new, data_from(item->key), data_from(item->value));
    }

    return new;
}