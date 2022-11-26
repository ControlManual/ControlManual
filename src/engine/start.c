#include <engine/lexer.h>
#include <core/ui.h>
#include <core/error.h>
#include <core/vector.h>
#include <engine/config.h>
#include <engine/commands.h>
#include <stdio.h>
#define PROCESS() if (process_errors(false)) { continue; }

void start() {
    ERRSTACK_INIT;
    ui* u = UI();
    load_config();
    init_types();
    load_commands();
    
    u->start();

    while (true) {
        vector* tokens = tokenize(u->input());
        PROCESS();
        char* command_name;
        vector* params = params_from_tokens(tokens, &command_name);
        PROCESS();
        if (!params) continue;

        command* c = map_get(commands, command_name);

        if (!c) {
            THROW_STATIC("unknown command", "<main>");
            process_errors(false);
            continue;
        }
        c->caller(params);
        process_errors(false);
    }
    ERRSTACK_FREE;
}
