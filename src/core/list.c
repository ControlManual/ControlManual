#include <core/list.h>
#include <core/util.h> // safe_malloc
#include <core/data.h>
#include <stdio.h>

static list_node* list_node_new(
    data* value,
    list_node* last,
    list_node* next
) {
    list_node* node = safe_malloc(sizeof(list_node));
    node->value = value;
    node->last = last;
    node->next = next;

    return node;
}

list* list_new(void) {
    list* l = safe_malloc(sizeof(list));
    l->first = NULL;
    l->len = 0;

    return l;
}

void list_append(list* l, data* value) {
    l->len++;
    if (l->first) {
        list_node* node = l->last;
        list_node* new = list_node_new(value, node, NULL);
        node->next = new;
        l->last = new;
    } else {
        list_node* node = list_node_new(value, NULL, NULL);
        l->first = node;
        l->last = node;
    }
}

static list_node* list_get_node(list* l, size_t index) {
    if (index >= l->len) return NULL;
    list_node* node = l->first;
    
    for (int i = 0; i < index; i++) {
        node = node->next;
    }

    return node;
}

inline void* list_get(list* l, size_t index) {
    return data_content_maybe(list_get_node(l, index)->value);
}

static inline void list_node_free(list_node* node) {
    data_free(node->value);
    free(node);
}

void list_remove(list* l, size_t index) {
    list_node* node = list_get_node(l, index);

    if (l->first == node) l->first = node->next;
    if (l->last == node) l->last = node->last;

    if (node->next) {
        node->next->last = node->last;
    }

    if (node->last) {
        node->last->next = node->next;
    }

    list_node_free(node);
    l->len--;
}

void list_free(list* l) {
    for (int i = 0; i < l->len; i++) {
        list_node_free(list_get_node(l, i));
    }

    free(l);
}