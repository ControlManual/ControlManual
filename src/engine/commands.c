#include <controlmanual/engine/commands.h>
#include <controlmanual/core/data.h>
#include <controlmanual/core/map.h>
#include <controlmanual/core/object.h>
#include <controlmanual/core/ui.h>
#include <controlmanual/engine/context.h> // parse_context
#include <controlmanual/engine/util.h>
#include <controlmanual/engine/config.h>
#include <controlmanual/engine/start.h>
#include <stdio.h> // sprintf
#include <string.h> // strlen
#define LOAD_ERROR(msg) { \
    char* str = safe_malloc(strlen(path) + (sizeof(msg) - 2)); \
    sprintf(str, msg, path); \
    THROW_HEAP(str, "<loading>"); \
    return; \
}
#define NOSYMBOL(sym) LOAD_ERROR(sym " is not exported by '%s'")

#ifdef COMMAND
#undef COMMAND
#endif

#define COMMAND(name, desc, ...) map_set(commands, STACK_DATA(#name), CUSTOM_DATA(command_new( \
        name##_impl, \
        schema_new( \
            STACK_DATA(#name), \
            STACK_DATA(desc), \
            param_array_from((param*[]) { __VA_ARGS__ }, NUMARGS(__VA_ARGS__)), \
            NUMARGS(__VA_ARGS__) \
        ) \
    ), command_dealloc));


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
    object* msg = NULL;
    if (!parse_context(c, &msg)) return NULL;
    u->print(STRING_VALUE(OBJECT_STR(msg)));

    return NULL;
}

object* exit_impl(context* c) {
    int status = 0;
    if (!parse_context(c, &status)) return NULL;
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

void command_dealloc(command* c) {
    data_free(c->sc->description);
    data_free(c->sc->name);
    for (int i = 0; i < c->sc->params_len; i++) {
        data_free(c->sc->params[i]->name);
        data_free(c->sc->params[i]->description);
        DATA_FREE_MAYBE(c->sc->params[i]->df);
        DATA_FREE_MAYBE(c->sc->params[i]->shorthand);
        free(c->sc->params[i]);
    }
    free(c->sc->params);
    free(c->sc);
    free(c);
}

void command_loader(char* path) {
    if (is_file(path)) {
        library l = OPEN_LIB(path);
        if (!l) LOAD_ERROR("couldn't load library at '%s'");

        param_construct_func pcf = GET_SYMBOL(l, "cm_param_construct");
        if (!pcf) NOSYMBOL("cm_param_construct")
        paramcontext* command_params = pcf();
        get_str_func name_func = GET_SYMBOL(l, "cm_command_name");
        if (!name_func) NOSYMBOL("cm_command_name")
        get_str_func desc_func = GET_SYMBOL(l, "cm_command_description");
        if (!desc_func) NOSYMBOL("cm_command_description")
        command_caller_func command_impl = GET_SYMBOL(l, "cm_command_caller");
        if (!command_impl) NOSYMBOL("cm_command_caller")

        data* name = name_func();
        data* desc = desc_func();
        
        map_set(
            commands,
            data_from(name),
            CUSTOM_DATA(
                command_new(command_impl, schema_new(
                    name,
                    desc,
                    command_params->params,
                    command_params->len
                )),
                command_dealloc
            )
        );

        free(command_params);
        CLOSE_LIB(l);
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
    free(p);
}

void iter_commands(map* m, commands_iter_func func) {
    for (int i = 0; i < m->capacity; i++) {
        pair* item = m->items[i];
        if (item) func(
            ((command*) data_content(item->value))->sc
        );
    }
}