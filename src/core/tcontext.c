#include <controlmanual/core/tcontext.h>
#include <controlmanual/core/error.h>
#include <stdbool.h>
#define ENUMNAME(name) case name: return #name;
#ifdef CM_DEBUG
#include <string.h>
#include <stdio.h>
#include <controlmanual/core/ui.h>
#endif

list* cm_tcontext_stack = NULL;
tcontext* cm_runtime_tcontext = NULL;

#ifdef CM_DEBUG
bool cm_show_tcontext = false;
#endif

static tcontext* tcontext_new(
    data* name,
    data* origin,
    tcontext* last,
    context* ctx,
    tcontext_state state
) {
    tcontext* tc = safe_malloc(sizeof(tcontext));
    tc->name = name;
    tc->origin = origin;
    tc->last = last;
    tc->ctx = ctx;
    tc->state = state;
    tc->finalizers_len = 0;
    tc->finalizers = NULL;
    return tc;
}

void tcontext_free(tcontext* tc) {
    for (int i = 0; i < tc->finalizers_len; i++) {
        tcontext_finalizer f = tc->finalizers[i];
        f(tc);
    }

    // ignore ctx here, its not the tcontext's job to free that
    DATA_FREE_MAYBE(tc->name);
    DATA_FREE_MAYBE(tc->origin);
    free(tc);
}

void tcontext_init() {
    cm_runtime_tcontext = tcontext_new(NULL, NULL, NULL, NULL, INITALIZING);
    cm_tcontext_stack = list_new();

    list_append(
        cm_tcontext_stack,
        CUSTOM_DATA(cm_runtime_tcontext, tcontext_free)
    );
}

const char* tcontext_state_name(tcontext_state state) {
    switch (state) {
        ENUMNAME(INITALIZING)
        ENUMNAME(COMMAND_LOAD)
        ENUMNAME(MIDDLEWARE_LOAD)
        ENUMNAME(PLUGIN_LOAD)
        ENUMNAME(COMMAND_LOOP)
        ENUMNAME(MIDDLEWARE_EXEC)
        ENUMNAME(COMMAND_EXEC)
        ENUMNAME(OBJECT_EXEC)
        ENUMNAME(FINALIZING)
    }

    return "<UNKNOWN>";
}

void tcontext_advance(
    data* name,
    data* origin,
    context* ctx,
    tcontext_state state
) {

    cm_runtime_tcontext = tcontext_new(
        name,
        origin,
        cm_runtime_tcontext,
        ctx,
        state
    );

    #ifdef CM_DEBUG
    if (cm_show_tcontext) {
        ui* u = ui_acquire();
        size_t len = snprintf(
            NULL,
            0,
            "advanced tcontext: (origin %s) (name %s) (address %p)",
            CONTENT_STR(origin),
            name ? CONTENT_STR(name) : "<null>",
            cm_runtime_tcontext
        );
        char* message = safe_malloc(len);
        sprintf(
            message,
            "advanced tcontext: (origin %s) (name %s) (address %p)",
            CONTENT_STR(origin),
            name ? CONTENT_STR(name) : "<null>",
            cm_runtime_tcontext
        );

        u->log(message, "tcontext_advance");
        free(message);
    }
    #endif

    list_append(
        cm_tcontext_stack,
        CUSTOM_DATA(cm_runtime_tcontext, tcontext_free)
    );
}


void tcontext_pop() {
    #ifdef CM_DEBUG
    if (cm_show_tcontext) {
        ui* u = ui_acquire();
        size_t len = snprintf(
            NULL,
            0,
            "popped tcontext: (origin %s) (name %s) (address %p)",
            CONTENT_STR(cm_runtime_tcontext->origin),
            cm_runtime_tcontext->name ? CONTENT_STR(cm_runtime_tcontext->name) : "<null>",
            cm_runtime_tcontext
        );
        char* message = safe_malloc(len);
        sprintf(
            message,
            "popped tcontext: (origin %s) (name %s) (address %p)",
            CONTENT_STR(cm_runtime_tcontext->origin),
            cm_runtime_tcontext->name ? CONTENT_STR(cm_runtime_tcontext->name) : "<null>",
            cm_runtime_tcontext
        );

        u->log(message, "tcontext_pop");
        free(message);
    }
    #endif
    process_errors();
    /*
        errors hold a reference to the tcontext,
        so we need to make sure everything is handled
        by the time its deallocated
    */
    cm_runtime_tcontext = list_get(
        cm_tcontext_stack,
        cm_tcontext_stack->len - 2
    );
    list_remove(cm_tcontext_stack, cm_tcontext_stack->len - 1);
}

void tcontext_add_finalizer(tcontext* tc, tcontext_finalizer finalizer) {
    if (!tc->finalizers_len) {
        tc->finalizers = safe_calloc(
            ++tc->finalizers_len,
            sizeof(tcontext_finalizer)
        );
        tc->finalizers[0] = finalizer;
    }

    tc->finalizers = safe_realloc(
        tc->finalizers,
        ++tc->finalizers_len * sizeof(tcontext_finalizer)
    );
    tc->finalizers[tc->finalizers_len - 1] = finalizer;
}

void tcontext_add_finalizer_inplace(tcontext_finalizer finalizer) {
    tcontext* tc = tcontext_acquire();
    tcontext_add_finalizer(tc, finalizer);
}

inline tcontext* tcontext_acquire() {
    if (!cm_runtime_tcontext) FAIL("tcontext is not initalized!");
    return cm_runtime_tcontext;
}
