#include <stdlib.h> // size_t
#include <core/vector.h>
#include <core/util.h>
#include <core/data.h>

/* Create a new vector. */
vector* vector_new(void) {
    vector* vec = safe_malloc(sizeof(vector));
    vec->size = 0;
    vec->items = safe_calloc(0, sizeof(data));
    return vec;
}

/* Get the data at the target index. */
data* vector_get_data(vector* vec, size_t index) {
    if (index >= vec->size) return NULL;
    return vec->items[index];
}

/* Free a vector. */
void vector_free(vector* vec) {
    for (int i = 0; i < vec->size; i++)
        data_free(vector_get_data(vec, i));
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
    vec->size++;
    data** items = safe_realloc(vec->items, vec->size * sizeof(data));
    for (int i = vec->size - 1; i >= index; i--)
        if (i == index) items[i] = item;
        else items[i] = items[i - 1];
    
    free(vec->items);
    vec->items = items;
};

/* Remove the index from the target vector and return its value. */
data* vector_pop(vector* vec, size_t index) {
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

/* Get the value at the target index. */
inline void* vector_get(vector* vec, size_t index) {
    return data_content_maybe(vector_get_data(vec, index));
}

inline void vector_set(vector* vec, size_t index, data* value) {
    data_free(vec->items[index]);
    vec->items[index] = value;
}