#include <core/error.h>
#include <core/list.h>
#include <core/data.h>
#include <core/util.h> // safe_malloc
#include <core/ui.h>
#include <stdbool.h>

list* error_stack = NULL;

void process_errors(ui* u, bool should_kill) {
    if (!error_stack->len) return;

    for (int i = 0; i < error_stack->len; i++) {
        error* e = list_get(error_stack, i);
        u->error(e);
        data_free(e->content);
        data_free(e->origin);
        list_remove(error_stack, i);
    }

    if (should_kill) exit(1);
}

inline error* error_pop(void) {
    return list_get(error_stack, error_stack->len - 1);
}

void throw(data* content, data* origin, error* cause) {
    error* e = safe_malloc(sizeof(error));
    e->content = content;
    e->origin = origin;
    e->cause = cause;
    list_append(error_stack, HEAP_DATA(e));
}