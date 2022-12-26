#ifndef CM_ERROR_H
#define CM_ERROR_H
#include <core/data.h>
#include <core/list.h>
#include <core/vector.h>
#include <stdbool.h>
#include <stdlib.h> // NULL
#define ERRSTACK_INIT error_stack = list_new()
#define ERRSTACK_FREE list_free(error_stack)
#define THROW_STATIC(content, where) throw( \
    STACK_DATA(content), \
    STACK_DATA(where), \
    NULL, \
    NULL, \
    NULL \
)
#define THROW_HEAP(content, where) throw(HEAP_DATA(content), STACK_DATA(where), NULL, NULL, NULL)

typedef struct STRUCT_ERROR error;
typedef FUNCTYPE(ui_error, void, (error*));
// this needs to be in here to avoid circular dependencies

struct STRUCT_ERROR {
    data* content;
    data* origin;
    error* cause;
    data* expr;
    vector* problems;
};

extern list* error_stack;

bool process_errors(bool should_kill);
void throw(
    data* content,
    data* origin,
    error* cause,
    data* expr,
    vector* problems
);

extern bool error_occurred();

#endif
