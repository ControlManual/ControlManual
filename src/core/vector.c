#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <core/vector.h>
#include <core/error.h>
#include <core/data.h>

/* Create a new vector. */
vector* vector_new(void) {
    vector* vec = safe_malloc(sizeof(vector));
    vec->size = 0;
    vec->items = safe_calloc(0, sizeof(data));
    return vec;
}

/* Free a vector. */
void vector_free(vector* vec) {
    for (int i = 0; i < vec->size; i++) {
        data* d = vector_get_data(vec, i);
        if (!d) process_errors();
        data_free(d);
    }
    free(vec->items);
    free(vec);
}

/* Append a value to the target vector. */
void vector_append(vector* vec, data* item) {
    vec->size++;
    vec->items = safe_realloc(vec->items, vec->size * sizeof(data));
    vec->items[vec->size - 1] = item;
};

/* Insert a value at the target index. */
void vector_insert(vector* vec, size_t index, data* item) {
    if (index > vec->size) fail("index is above vector size");

    vec->size++;
    vec->items = safe_realloc(vec->items, vec->size * sizeof(data));
    for (int i = vec->size - 1; i >= index; i--)
        if (i == index) vec->items[i] = item;
        else vec->items[i] = vec->items[i - 1];
};

/* Remove the index from the target vector and return its value. */
data* vector_pop(vector* vec, size_t index) {
    if (index > vec->size) fail("index is above vector size");
    void* item = vec->items[index];
    data** new_items = safe_calloc(vec->size - 1, sizeof(data));
    size_t offset = 0;

    for (int i = 0; i < vec->size; i++) 
        if (i != index) new_items[i - offset] = vec->items[i];
        else offset++;

    free(vec->items);
    vec->items = new_items;
    vec->size--;

    return item;
}

/* Remove the index from the target vector and free its value. */
inline void vector_remove(vector* vec, size_t index) {
    data_free(vector_pop(vec, index));
}

/* Get the data at the target index. */
inline data* vector_get_data(vector* vec, size_t index) {
    if (index >= vec->size) fail("vector index out of range");
    return vec->items[index];
}

/* Get the value at the target index. */
inline void* vector_get(vector* vec, size_t index) {
    data* d = vector_get_data(vec, index);
    return d->contents;
}

inline void vector_set(vector* vec, size_t index, data* value) {
    data_free(vec->items[index]);
    vec->items[index] = value;
}