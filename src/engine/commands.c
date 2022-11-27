#include <engine/commands.h>
#include <core/data.h>
#include <core/map.h>
#include <core/object.h>
#include <core/ui.h>

#define COMMAND(name, desc, numargs, ...) map_set(commands, STACK_DATA(#name), HEAP_DATA(command_new( \
        name##_impl, \
        schema_new( \
            STACK_DATA(#name), \
            STACK_DATA(desc), \
            param_array_from((param*[]) { __VA_ARGS__ }), \
            numargs \
        ), \
        settings \
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
    return NULL;
}

object* exit_impl(vector* params) {
    ui* u = UI();
    int status = 0;
    if (!parse_args(params, "|i", &status)) return NULL;
    if (u->end) u->end();
    exit(status);
}

object* help_impl(void) {
    ui* u = UI();
    u->help(commands);
    return NULL;
}

param* param_new(
    data* name,
    data* description,
    type* tp,
    bool flag,
    bool keyword,
    bool required,
    data* df
) {
    param* p = safe_malloc(sizeof(param));
    p->name = name;
    p->description = description;
    p->tp = tp;
    p->flag = flag;
    p->keyword = keyword;
    p->required = required;
    p->df = df;
    return p;
}

param** param_array_from(param** array) {
    size_t size = sizeof(array) / sizeof(array[0]);
    param** a = safe_calloc(size, sizeof(param));

    for (int i = 0; i < size; i++)
        a[i] = array[i];
    
    return a;
};

void load_commands(void) {
    commands = map_new(1);
    COMMAND(
        echo,
        "Print output",
        1,
        PARAM("msg", "Content to print.", string)
    );
    COMMAND(
        exit,
        "Exit Control Manual.",
        1,
        PARAM(
            "status",
            "Status code to exit with.",
            integer
        )
    )
    
    COMMAND(help, "Display help menu.", 0);
}

void iter_commands(map* m, commands_iter_func func) {
    for (int i = 0; i < m->capacity; i++) {
        pair* item = m->items[i];
        if (item) func(
            ((command*) data_content(item->value))->sc
        );
    }
}