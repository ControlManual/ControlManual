#include <controlmanual/core/ui.h>
#include <controlmanual/core/error.h>
#include <controlmanual/core/vector.h>
#include <controlmanual/core/object.h>
#include <controlmanual/core/tcontext.h>
#include <controlmanual/engine/context.h>
#include <controlmanual/engine/lexer.h>
#include <controlmanual/engine/main.h>
#include <controlmanual/engine/config.h>
#include <controlmanual/engine/loader.h>
#include <string.h> // strdup
#include <stdio.h>
#include <signal.h>
#include <setjmp.h>

#define PROCESS() if (process_errors()) continue;
#define PROCESS_EXEC() if (error_occurred()) return NULL;
#undef PRINT
#define PRINT(o) { \
    object* str = object_to_string(o); \
    PROCESS(); \
    u->print(STRING_VALUE(str)); \
}

char* PATH;
char* cm_dir;

void unload() {
    ADVANCE_DEFAULT(NULL, unload, FINALIZING);
    ui* u = UI();
    scope_free(GLOBAL, false);
    free(PATH);
    free(cm_dir);
    map_free(commands);
    vector_free(cm_middleware);
    ERRSTACK_FREE;
    if (u->end) u->end();
    while (cm_runtime_tcontext) tcontext_pop();
    list_free(cm_tcontext_stack);
    free(u);
    exit(EXIT_SUCCESS);
}

void sigint(int signum) {
    exit(EXIT_SUCCESS);
}

#ifdef CM_DEBUG
void sigsegv(int signum) {
    FAIL("segmentation fault");
}
#endif

object* command_exec(const char* str) {
    vector* tokens = tokenize(str);
    PROCESS_EXEC();
    data* command_name_data;

    vector* params = vector_new();
    vector* flags = vector_new();
    map* keywords = map_new(2);

    params_from_tokens(tokens, &command_name_data, params, flags, keywords);
    PROCESS_EXEC();
    if (!params) return NULL;

    char* command_name = ((token*) data_content(command_name_data))->content;
    command* c = map_get(commands, command_name);

    if (!c) {
        THROW_STATIC("unknown command", "<main>");
        process_errors();
        return NULL;
    }

    jmp_buf buf;
    object* o = NULL;
    if (setjmp(buf)) {
        // context aborted
        goto end;
    };

    context* co = context_new(c, params, flags, keywords, &buf);
    ADVANCE_DEFAULT_CTX(
        NOFREE_DATA(command_name),
        command_exec,
        co,
        COMMAND_EXEC
    );
    tcontext* tc = TC();

    for (int i = 0; i < VECTOR_LENGTH(cm_middleware); i++)
        ((middleware*) VECTOR_GET(cm_middleware, i))->func(tc, co);

    o = c->caller(co);
end:
    tcontext_pop();
    context_free(co);
    process_errors();

    return o;
}

void start() {
    ERRSTACK_INIT;
    ui* u = UI();
    cm_dir = cat_path(home(), ".controlmanual");
    if (!exists(cm_dir)) create_dir(cm_dir);
    tcontext_init();
    init_types();
    process_errors();

    GLOBAL = scope_new();
    PATH = strdup(home());
    if (u->start) u->start();
    atexit(unload);
    signal(SIGINT, sigint);
#ifdef CM_DEBUG
    signal(SIGSEGV, sigsegv);
#endif
    load_commands();
    load_middleware();
    load_plugins();
    load_config();
    process_errors();

    ADVANCE_DEFAULT(NULL, start, COMMAND_LOOP);
    while (true) {
        data* d = u->input(GLOBAL_ACCESS, NULL);
        vector* tokens = tokenize(data_content(d));
        PROCESS();
        data* command_name_data = NULL;

        vector* params = vector_new();
        vector* flags = vector_new();
        map* keywords = map_new(2);

        params_from_tokens(tokens, &command_name_data, params, flags, keywords);
        PROCESS();
        if (!command_name_data) continue;
        char* command_name = (
            (token*) data_content(command_name_data)
        )->content;

        command* c = map_get(commands, command_name);

        if (!c) {
            DISALLOW_ERRORS;
            object* var = scope_get(GLOBAL, command_name);
            if (var) {
                PRINT(var);
                continue;
            }
            ALLOW_ERRORS;

            THROW_STATIC("unknown command", "<main>");
            process_errors();
            continue;
        }

        jmp_buf buf;

        if (setjmp(buf)) {
            // context aborted
            goto end;
        };

        context* co = context_new(c, params, flags, keywords, &buf);
        ADVANCE_DEFAULT_CTX(NOFREE_DATA(command_name), start, co, COMMAND_EXEC);
        object* result = NULL; // in case the function doesnt return
        tcontext* tc = TC();

        ADVANCE_DEFAULT_CTX(NULL, start, co, MIDDLEWARE_EXEC);
        for (int i = 0; i < VECTOR_LENGTH(cm_middleware); i++)
            ((middleware*) VECTOR_GET(cm_middleware, i))->func(tc, co);

        tcontext_pop();
        result = c->caller(co);

end:
        tcontext_pop();
        context_free(co);
        process_errors();
        if (result) PRINT(result);
        command_name = NULL;
        data_free(d);
        data_free(command_name_data);
    }
}
