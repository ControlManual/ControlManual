#include <controlmanual/core/vector.h>
#include <controlmanual/core/error.h>
#include <controlmanual/core/list.h>
#include <controlmanual/core/data.h>
#include <controlmanual/core/util.h> // safe_malloc
#include <controlmanual/core/ui.h>
#include <stdbool.h>
#include <errno.h>
#include <math.h> // log10, ceil
#include <string.h> // strerror
#include <stdio.h> // sprintf

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
    data* expr,
    vector* problems
) {
    if (errors_suppressed) return;
    error* e = safe_malloc(sizeof(error));
    tcontext* tc = TC();
    e->content = content;
    e->origin = origin;
    e->expr = expr;
    e->problems = problems;
    e->tc = tc;
    list_append(error_stack, HEAP_DATA(e));
}

void throw_errno(data* origin) {
    if (errors_suppressed) return;
    char* str = strerror(errno);
    char* err = safe_malloc(((int) ceil(log10(errno + 2)) + strlen(str) + 9));
    sprintf(err, "errno %d: %s", errno, str);
    throw_error(HEAP_DATA(err), origin, NULL, NULL);
}

void* generic_error(void) {
    if (!error_occurred()) {
        tcontext* tc = tcontext_acquire();
        throw_error(
            STACK_DATA(CONTENT_STR(tc->name)),
            STACK_DATA("An error occurred!"),
            NULL,
            NULL
        );
    }
    return NULL;
}
