#ifndef CM_TCONTEXT_H
#define CM_TCONTEXT_H

#include <controlmanual/core/data.h>
#include <controlmanual/core/util.h> // FUNCTYPE
#include <controlmanual/core/list.h>

#define ADVANCE_DEFAULT(name, state) \
    tcontext_advance(name, STACK_DATA((void*) __func__), NULL, state)
#define ADVANCE_DEFAULT_CTX(name, ctx, \
                            state) \
    tcontext_advance(name, STACK_DATA((void*) __func__), ctx, state)
#define TC() tcontext_acquire()
#define FINALIZE(finalizer) tcontext_add_finalizer_inplace(finalizer)

typedef struct STRUCT_CONTEXT context;

typedef enum ENUM_TCONTEXT_STATE {
    INITALIZING, /* runtime is still initalizing */
    COMMAND_LOAD, /* command is being loaded */
    MIDDLEWARE_LOAD, /* middleware is being loaded */
    PLUGIN_LOAD, /* plugin is being loaded */
    PACKAGE_LOAD, /* package is being loaded */
    COMMAND_LOOP, /* runtime is ready, we are now in the command loop */
    MIDDLEWARE_EXEC, /* we are executing middleware */
    COMMAND_EXEC, /* we are executing a command */
    OBJECT_EXEC, /* we are executing something on an object */
    FINALIZING, /* runtime is shutting down */
} tcontext_state;

typedef struct STRUCT_TCONTEXT tcontext;
typedef FUNCTYPE(tcontext_finalizer, void, (tcontext*));

struct STRUCT_TCONTEXT {
    data* name;
    data* origin;
    tcontext* last;
    context* ctx;
    tcontext_state state;
    tcontext_finalizer* finalizers;
    size_t finalizers_len;
};

extern list* cm_tcontext_stack;
extern tcontext* cm_runtime_tcontext;

#ifdef CM_DEBUG
#include <stdbool.h>
extern bool cm_show_tcontext;
#endif

API void tcontext_init();
API void tcontext_free(tcontext* tc);
API void tcontext_advance(
    data* name,
    data* origin,
    context* ctx,
    tcontext_state state
);
API void tcontext_pop();
API extern tcontext* tcontext_acquire();

API void tcontext_add_finalizer(tcontext* tc, tcontext_finalizer finalizer);
API void tcontext_add_finalizer_inplace(tcontext_finalizer finalizer);
API const char* tcontext_state_name(tcontext_state state);

#endif
