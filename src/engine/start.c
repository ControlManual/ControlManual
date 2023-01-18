#include <controlmanual/engine/lexer.h>
#include <controlmanual/core/ui.h>
#include <controlmanual/core/error.h>
#include <controlmanual/core/vector.h>
#include <controlmanual/engine/config.h>
#include <controlmanual/engine/commands.h>
#include <controlmanual/core/object.h>
#include <controlmanual/engine/context.h>
#include <controlmanual/engine/start.h>
#include <string.h> // strdup
#include <stdio.h>
#include <signal.h>

#define PROCESS() if (process_errors(false)) { continue; }
#define PROCESS_EXEC() if (error_occurred()) return;
#define PRINT(o) { \
    object* str = OBJECT_STR(o); \
    PROCESS(); \
    u->print(STRING_VALUE(str)); \
}

char* PATH;

void unload() {
    ui* u = UI();
    scope_free(GLOBAL, false);
    free(PATH);
    free(cm_dir);
    map_free(commands);
    ERRSTACK_FREE;
    if (u->end) u->end();
    free(u);
}

void sigint(int signum) {
    exit(0);
}

void command_exec(char* str) {
    vector* tokens = tokenize(str);
    PROCESS_EXEC();
    data* command_name_data;
    
    vector* params = vector_new();
    vector* flags = vector_new();
    map* keywords = map_new(2);
    
    params_from_tokens(tokens, &command_name_data, params, flags, keywords);
    PROCESS_EXEC();
    if (!params) return;

    char* command_name = ((token*) data_content(command_name_data))->content;
    command* c = map_get(commands, command_name);

    if (!c) {
        THROW_STATIC("unknown command", "<main>");
        process_errors(false);
        return;
    }

    context* co = context_new(c, params, flags, keywords);
    c->caller(co);
    context_free(co);
    process_errors(false);
}

void start() {
    ERRSTACK_INIT;
    ui* u = UI();
    load_config();
    init_types();
    process_errors(false);

    GLOBAL = scope_new();
    PATH = strdup(home());
    u->start();
    load_commands();
    atexit(unload);
    signal(SIGINT, sigint);
    process_errors(false);

    while (true) {
        data* d = u->input();
        vector* tokens = tokenize(data_content(d));
        PROCESS();
        data* command_name_data = NULL;
        
        vector* params = vector_new();
        vector* flags = vector_new();
        map* keywords = map_new(2);
        
        params_from_tokens(tokens, &command_name_data, params, flags, keywords);
        PROCESS();
        if (!command_name_data) continue;
        char* command_name = ((token*) data_content(command_name_data))->content;

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
            process_errors(false);
            continue;
        }

        context* co = context_new(c, params, flags, keywords);
        object* result = c->caller(co);
        context_free(co);
        process_errors(false);
        if (result) PRINT(result);
        command_name = NULL;
        data_free(d);
        data_free(command_name_data);
    }
}
