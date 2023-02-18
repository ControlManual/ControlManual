#include <controlmanual/core/vector.h>
#include <controlmanual/core/error.h>
#include <controlmanual/core/list.h>
#include <controlmanual/core/data.h>
#include <controlmanual/core/util.h> // safe_malloc
#include <controlmanual/core/ui.h>
#include <stdbool.h>

list* error_stack = NULL;
bool errors_suppressed = false;

inline bool error_occurred() {
    return error_stack->len ? true : false;
}

bool process_errors() {
    ui* u = UI();
    if (!error_stack->len) return false;

    for (int i = 0; i < error_stack->len; i++) {
        error* e = list_get(error_stack, i);
        u->error(e, e->tc);
        data_free(e->content);
        data_free(e->origin);
        list_remove(error_stack, i);
    }

    return true;
}

inline error* error_pop(void) {
    return list_get(error_stack, error_stack->len - 1);
}

void throw_error(
    data* content,
    data* origin,
    error* cause,
    data* expr,
    vector* problems
) {
    if (errors_suppressed) return;
    error* e = safe_malloc(sizeof(error));
    tcontext* tc = TC();
    e->content = content;
    e->origin = origin;
    e->cause = cause;
    e->expr = expr;
    e->problems = problems;
    e->tc = tc;
    list_append(error_stack, HEAP_DATA(e));
}
