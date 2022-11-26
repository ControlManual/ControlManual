#include <engine/commands.h>
#include <core/data.h>
#include <core/map.h>
#include <core/object.h>
#include <core/ui.h>
#define PARAM(name, desc, tp) param_new(STACK_DATA(#name), STACK_DATA(desc), &tp)
#define COMMAND(name, desc, numargs, ...) map_set(commands, STACK_DATA(#name), HEAP_DATA(command_new( \
        name##_impl, \
        schema_new( \
            STACK_DATA(#name), \
            STACK_DATA(desc), \
            (param*[]) { __VA_ARGS__ }, \
            numargs \
        ) \
    )));


map* commands;

schema* schema_new(data* name, data* description, param** params, size_t params_len) {
    schema* s = safe_malloc(sizeof(schema));
    s->name = name;
    s->description = description;
    s->params = params;
    s->params_len = params_len;
    return s;
}

command* command_new(command_caller caller, schema* sc) {
    command* c = safe_malloc(sizeof(command));
    c->caller = caller;
    c->sc = sc;
    return c;
}

object* echo_impl(vector* params) {
    ui* u = UI();
    char* msg;

    if (!parse_args(params, "s", &msg)) return NULL;
    u->print(msg);
}

object* exit_impl(vector* params) {
    ui* u = UI();
    int status = 0;
    if (!parse_args(params, "|i", &status)) return NULL;
    if (u->end) u->end();
    exit(status);
}

object* help_impl(vector* params) {
    ui* u = UI();
    u->help(commands);
}

param* param_new(data* name, data* description, type* tp) {
    param* p = safe_malloc(sizeof(param));
    p->name = name;
    p->description = description;
    p->tp = tp;
    return p;
}

void load_commands(void) {
    commands = map_new(1);
    COMMAND(echo, "Print output", 1, PARAM("msg", "Content to print.", string));
    COMMAND(exit, "Exit Control Manual.", 1, PARAM("status", "Status code to exit with.", integer))
    COMMAND(help, "Display help menu.", 0);
}
#include <stdio.h>
void iter_commands(map* m, commands_iter_func func) {
    for (int i = 0; i < m->capacity; i++) {
        pair* item = m->items[i];
        if (item) func(
            ((command*) data_content(item->value))->sc
        );
    }
}