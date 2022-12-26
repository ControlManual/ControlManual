#include <core/error.h>
#include <core/list.h>
#include <core/data.h>
#include <core/util.h> // safe_malloc
#include <core/ui.h>
#include <stdbool.h>
#include <core/vector.h>

list* error_stack = NULL;

inline bool error_occurred() {
    return error_stack->len ? true : false;
}

bool process_errors(bool should_kill) {
    ui* u = UI();
    if (!error_stack->len) return false;

    for (int i = 0; i < error_stack->len; i++) {
        error* e = list_get(error_stack, i);
        u->error(e);
        data_free(e->content);
        data_free(e->origin);
        list_remove(error_stack, i);
    }

    if (should_kill) exit(1);
    return true;
}

inline error* error_pop(void) {
    return list_get(error_stack, error_stack->len - 1);
}

void throw(
    data* content,
    data* origin,
    error* cause,
    data* expr,
    vector* problems
) {
    error* e = safe_malloc(sizeof(error));
    e->content = content;
    e->origin = origin;
    e->cause = cause;
    e->expr = expr;
    e->problems = problems;
    list_append(error_stack, HEAP_DATA(e));
}
