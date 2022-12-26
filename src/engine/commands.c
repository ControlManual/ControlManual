#include <engine/commands.h>
#include <core/data.h>
#include <core/map.h>
#include <core/object.h>
#include <core/ui.h>
#include <engine/context.h> // parse_context
#include <engine/util.h>
#include <stdio.h> // sprintf
#include <engine/config.h>
#include <string.h> // strlen

#ifdef COMMAND
#undef COMMAND
#endif

#define COMMAND(name, desc, ...) map_set(commands, STACK_DATA(#name), HEAP_DATA(command_new( \
        name##_impl, \
        schema_new( \
            STACK_DATA(#name), \
            STACK_DATA(desc), \
            param_array_from((param*[]) { __VA_ARGS__ }, NUMARGS(__VA_ARGS__)), \
            NUMARGS(__VA_ARGS__) \
        ) \
    )));


/* Dummy type for representing "any" in schemas. Don't use as an actual type. */
type cm_any_wrapper = {};

map* commands;

paramcontext* paramcontext_new(param** params, size_t len) {
    paramcontext* pc = safe_malloc(sizeof(paramcontext));
    pc->len = len;
    pc->params = params;
    return pc;
}

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

object* echo_impl(context* c) {
    ui* u = UI();
    object* msg;
    if (!parse_context(c, &msg)) return NULL;
    u->print(STRING_VALUE(OBJECT_STR(msg)));

    return NULL;
}

object* exit_impl(context* c) {
    ui* u = UI();
    int status = 0;
    if (!parse_context(c, &status)) return NULL;
    if (u->end) u->end();
    exit(status);
}

object* help_impl(context* c) {
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
    data* df,
    bool convert,
    data* shorthand
) {
    param* p = safe_malloc(sizeof(param));
    p->name = name;
    p->description = description;
    p->tp = tp;
    p->flag = flag;
    p->keyword = keyword;
    p->required = required;
    p->df = df;
    p->convert = convert;
    p->shorthand = shorthand;
    return p;
}
param** param_array_from(param** array, size_t size) {
    param** a = safe_calloc(size, sizeof(param*));

    for (int i = 0; i < size; i++) 
        a[i] = array[i];
    
    return a;
};

void command_loader(char* path) {
    if (is_file(path)) {
        library l = OPEN_LIB(path);
        if (!l) {
            char* str = safe_malloc(strlen(path) + 28);
            sprintf(str, "couldn't load library at '%s'", path);
            THROW_HEAP(str, "<loading>");
            return;
        }

        param_construct_func pcf = GET_SYMBOL(l, "cm_param_construct");
        paramcontext* command_params = pcf();
        char* name = ((get_str_func) GET_SYMBOL(l, "cm_command_name"))();
        char* desc = ((get_str_func) GET_SYMBOL(l, "cm_command_description"))();
        command_caller_func command_impl = GET_SYMBOL(l, "cm_command_caller");
        
        map_set(
            commands,
            NOFREE_DATA(name),
            HEAP_DATA(
                command_new(command_impl, schema_new(
                    NOFREE_DATA(name),
                    NOFREE_DATA(desc),
                    command_params->params,
                    command_params->len
                ))
            )
        );

        free(command_params);
    }
}

void load_commands(void) {
    commands = map_new(1);

    COMMAND(
        echo,
        "Print output.",
        ARG("msg", "Content to print.", any),
    );

    COMMAND(
        exit,
        "Exit Control Manual.",
        DEFAULT_ARG(
            "status",
            "Status code to exit with.",
            integer,
            0
        )
    )
    
    COMMAND(help, "Display help menu.");

    char* p = cat_path(cm_dir, "commands");
    if (!exists(p)) create_dir(p);
    else iterate_dir(p, command_loader);
}

void iter_commands(map* m, commands_iter_func func) {
    for (int i = 0; i < m->capacity; i++) {
        pair* item = m->items[i];
        if (item) func(
            ((command*) data_content(item->value))->sc
        );
    }
}