#include <engine/lexer.h>
#include <core/ui.h>
#include <core/error.h>
#include <core/vector.h>
#include <engine/config.h>
#include <engine/commands.h>
#include <core/object.h>
#include <stdio.h>
#include <engine/context.h>
#include <engine/start.h>

#define PROCESS() if (process_errors(false)) { continue; }
#define PRINT(o) { \
    object* str = OBJECT_STR(o); \
    PROCESS(); \
    u->print(STRING_VALUE(str)); \
}

scope* GLOBAL;
char* PATH;

void start() {
    ERRSTACK_INIT;
    ui* u = UI();
    load_config();
    init_types();
    load_commands();
    process_errors(false);

    GLOBAL = scope_new();
    PATH = "/home/zero";
    u->start();

    while (true) {
        vector* tokens = tokenize(u->input());
        PROCESS();
        char* command_name;
        
        vector* params = vector_new();
        vector* flags = vector_new();
        map* keywords = map_new(2);
        
        params_from_tokens(tokens, &command_name, params, flags, keywords);
        PROCESS();
        if (!params) continue;

        command* c = map_get(commands, command_name);

        if (!c) {
            object* var = scope_get(GLOBAL, command_name);
            if (var) {
                PRINT(var);
                continue;
            }
            THROW_STATIC("unknown command", "<main>");
            process_errors(false);
            continue;
        }

        context* co = context_new(c, params, flags, keywords);
        object* result = c->caller(co);
        context_free(co);
        process_errors(false);
        if (result) PRINT(result);
    }
    ERRSTACK_FREE;
}
