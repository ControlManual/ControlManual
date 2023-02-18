#include <controlmanual/core/vector.h>
#include <controlmanual/core/util.h>
#include <controlmanual/core/data.h>
#include <stdlib.h> // size_t

/* Create a new vector. */
vector* vector_new(void) {
    vector* vec = safe_malloc(sizeof(vector));
    vec->items = safe_calloc(0, data_sizeof());
    vec->size = 0;
    return vec;
}

/* Get the data at the target index. */
data* vector_get_data(vector* vec, size_t index) {
    if (index >= VECTOR_LENGTH(vec)) return NULL;
    return vec->items[index];
}

/* Get the data at the target index and build a new data object. */
inline data* vector_get_data_new(vector* vec, size_t index) {
    data* result = vector_get_data(vec, index);
    return result ? data_from(result) : NULL;
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
    vec->items = safe_realloc(vec->items, vec->size * sizeof(data*));
    vec->items[vec->size - 1] = item;
};

/* Insert a value at the target index. */
void vector_insert(vector* vec, size_t index, data* item) {
    vec->size++;
    data** items = safe_realloc(vec->items, vec->size * sizeof(data*));

    for (int i = vec->size - 1; i >= index; i--) {
        /* i dont really remember why i made this loop backwards, 
        but its still O(N) so i guess its fine */
        if (i == index) {
            items[i] = item;
            break;
        }
        else items[i] = items[i - 1];
    }

    vec->items = items;
};

/* Remove the index from the target vector and return its value. */
data* vector_pop(vector* vec, size_t index) {
    void* item = vec->items[index];
    data** new_items = safe_calloc(vec->size - 1, sizeof(data*));
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

vector* vector_copy(const vector* vec) {
    vector* new = safe_malloc(sizeof(vector));
    new->items = safe_calloc(VECTOR_LENGTH(vec), data_sizeof());
    new->size = vec->size;

    for (int i = 0; i < VECTOR_LENGTH(vec); i++)
        new->items[i] = data_from(VECTOR_GET_DATA(vec, i));

    return new;
}