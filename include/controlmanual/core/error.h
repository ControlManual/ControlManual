#ifndef CM_ERROR_H
#define CM_ERROR_H
#include <controlmanual/core/data.h>
#include <controlmanual/core/list.h>
#include <controlmanual/core/vector.h>
#include <controlmanual/core/tcontext.h>
#include <controlmanual/core/util.h>
#include <stdbool.h>
#include <stdlib.h> // NULL
#define ERRSTACK_INIT error_stack = list_new()
#define ERRSTACK_FREE list_free(error_stack)
#define THROW_STATIC(content, where) throw_error( \
    STACK_DATA(content), \
    STACK_DATA(where), \
    NULL, \
    NULL \
)
#define THROW_HEAP(content, where) throw_error(HEAP_DATA(content), STACK_DATA(where), NULL, NULL)

typedef struct STRUCT_ERROR error;
typedef FUNCTYPE(ui_error, void, (error*, tcontext*));
// this needs to be in here to avoid circular dependencies

struct STRUCT_ERROR {
    data* content;
    data* origin;
    data* expr;
    vector* problems;
    tcontext* tc;
};

extern list* error_stack;
extern bool errors_suppressed;

API bool process_errors();
API void throw_error(
    data* content,
    data* origin,
    data* expr,
    vector* problems
);

API extern bool error_occurred();

#define DISALLOW_ERRORS { errors_suppressed = true
#define ALLOW_ERRORS errors_suppressed = false; }

#endif
