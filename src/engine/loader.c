#include <controlmanual/engine/loader.h>
#include <controlmanual/core/data.h>
#include <controlmanual/core/map.h>
#include <controlmanual/core/object.h>
#include <controlmanual/core/ui.h>
#include <controlmanual/engine/context.h> // parse_context
#include <controlmanual/engine/util.h>
#include <controlmanual/engine/config.h>
#include <controlmanual/engine/main.h>
#include <controlmanual/core/tcontext.h>
#include <stdio.h> // sprintf
#include <string.h> // strlen
#define LOAD_ERROR(msg) { \
    char* str = safe_malloc(strlen(path) + (sizeof(msg) - 2)); \
    sprintf(str, msg, path); \
    THROW_HEAP(str, "<loading>"); \
    tcontext_pop(); \
    return; \
}
#define NOSYMBOL(sym) LOAD_ERROR(sym " is not exported by '%s'")

#ifdef COMMAND
#undef COMMAND
#endif

#define COMMAND(name, desc, ...) map_set( \
    commands, \
    STACK_DATA(#name), \
    CUSTOM_DATA(command_new( \
        name##_impl, \
        NULL, \
        schema_new( \
            STACK_DATA(#name), \
            STACK_DATA(desc), \
            param_array_from( \
                (param*[]) { __VA_ARGS__ }, \
                NUMARGS(__VA_ARGS__) \
            ), \
            NUMARGS(__VA_ARGS__) \
        ) \
    ), command_dealloc));


/*
    Dummy type for representing "any" in schemas.
    Do not use as an actual type.
*/
type cm_any_wrapper = {};

map* commands;

void middleware_loader(char* path);
void plugin_loader(char* path);
void command_loader(char* path);
// these are needed to stop the compiler from complaining

paramcontext* paramcontext_new(param** params, size_t len) {
    paramcontext* pc = safe_malloc(sizeof(paramcontext));
    pc->len = len;
    pc->params = params;
    return pc;
}

schema* schema_new(
    data* name,
    data* description,
    param** params,
    size_t params_len
) {
    schema* s = safe_malloc(sizeof(schema));
    s->name = name;
    s->description = description;
    s->params = params;
    s->params_len = params_len;
    return s;
}

command* command_new(command_caller caller, library lib, schema* sc) {
    command* c = safe_malloc(sizeof(command));
    c->caller = caller;
    c->lib = lib;
    c->sc = sc;
    return c;
}

object* echo_impl(context* c) {
    object* msg;
    parse_context(c, &msg);
    print_obj(msg);

    return NULL;
}

object* exit_impl(context* c) {
    int status = 0;
    parse_context(c, &status);
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
    data* shorthand,
    bool option,
    size_t noptions,
    const char** options
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
    p->option = option;
    p->options_size = noptions;
    p->options = safe_calloc(noptions, sizeof(const char*));

    for (int i = 0; i < noptions; i++)
        p->options[i] = options[i];

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
        if (c->sc->params[i]->options) free(c->sc->params[i]->options);
        free(c->sc->params[i]);
    }
    free(c->sc->params);
    free(c->sc);
    if (c->lib) CLOSE_LIB(c->lib);
    free(c);
}

void command_loader(char* path) {
    ADVANCE_DEFAULT(NOFREE_DATA(path), command_loader, COMMAND_LOAD);
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
        command_caller command_impl = GET_SYMBOL(l, "cm_command_caller");
        if (!command_impl) NOSYMBOL("cm_command_caller")
        plugin_impl_func init_func = GET_SYMBOL(l, "cm_command_init");
        if (init_func) init_func();

        data* name = name_func();
        data* desc = desc_func();

        if (map_get(commands, CONTENT_STR(name))) {
            ui* u = UI();
            char* buf = safe_malloc(strlen(CONTENT_STR(name)) + 32);
            sprintf(buf, "overwriting existing command: %s", CONTENT_STR(name));
            u->warn(buf);
            free(buf);
        }

        map_set(
            commands,
            data_from(name),
            CUSTOM_DATA(
                command_new(
                    command_impl,
                    l,
                    schema_new(
                        name,
                        desc,
                        command_params->params,
                        command_params->len
                    )
                ),
                command_dealloc
            )
        );
        free(command_params);

        sym_flags_func flags_func = GET_SYMBOL(l, "cm_symbol_flags");
        if (flags_func) {
            size_t flags = flags_func();
            if ((flags & IS_PLUGIN) == IS_PLUGIN)
                plugin_loader(path);
            if ((flags & IS_MIDDLEWARE) == IS_MIDDLEWARE)
                middleware_loader(path);
        }
    }
    tcontext_pop();
}

void plugin_loader(char* path) {
    ui* u = UI();
    ADVANCE_DEFAULT(NOFREE_DATA(path), plugin_loader, PLUGIN_LOAD);

    if (is_file(path)) {
        library l = OPEN_LIB(path);
        if (!l) LOAD_ERROR("couldn't load library at '%s'");
        
        get_str_func name_func = GET_SYMBOL(l, "cm_plugin_name");
        if (!name_func) NOSYMBOL("cm_plugin_name")
        plugin_impl_func plugin_func = GET_SYMBOL(l, "cm_plugin_func");
        if (!plugin_func) NOSYMBOL("cm_plugin_func")

        data* name = name_func();
        plugin_func();

        char* str = safe_malloc(17 + strlen(CONTENT_STR(name)));
        sprintf(str, "loaded plugin \"%s\"", CONTENT_STR(name));
        u->alert(str);
        free(str);

        sym_flags_func flags_func = GET_SYMBOL(l, "cm_symbol_flags");
        if (flags_func) {
            size_t flags = flags_func();
            if ((flags & IS_COMMAND) == IS_COMMAND)
                command_loader(path);
            if ((flags & IS_MIDDLEWARE) == IS_MIDDLEWARE)
                middleware_loader(path);
        }

        CLOSE_LIB(l);
    }
    tcontext_pop();
}

vector* cm_middleware = NULL;

static middleware* middleware_new(library l, middleware_impl func) {
    middleware* m = safe_malloc(sizeof(middleware));
    m->lib = l;
    m->func = func;
    return m;
}

static void middleware_dealloc(middleware* m) {
    CLOSE_LIB(m->lib);
    free(m);
}

void middleware_loader(char* path) {
    if (!cm_middleware) cm_middleware = vector_new();
    // in case the loader gets called early

    ADVANCE_DEFAULT(NOFREE_DATA(path), middleware_loader, MIDDLEWARE_LOAD);

    if (is_file(path)) {
        library l = OPEN_LIB(path);
        if (!l) LOAD_ERROR("couldn't load library at '%s'");

        middleware_impl func = GET_SYMBOL(l, "cm_middleware_func");
        if (!func) NOSYMBOL("cm_middleware_func");

        vector_append(
            cm_middleware,
            CUSTOM_DATA(middleware_new(l, func), middleware_dealloc)
        );

        sym_flags_func flags_func = GET_SYMBOL(l, "cm_symbol_flags");
        if (flags_func) {
            size_t flags = flags_func();
            if ((flags & IS_COMMAND) == IS_COMMAND)
                command_loader(path);
            if ((flags & IS_PLUGIN) == IS_PLUGIN)
                plugin_loader(path);
        }
    }
    tcontext_pop();
}

void load_commands(void) {
    ADVANCE_DEFAULT(NULL, load_commands, COMMAND_LOAD);
    commands = map_new(4);

    COMMAND(
        echo,
        "Print output.",
        ARG(msg, "Content to print.", any),
    );

    COMMAND(
        exit,
        "Exit Control Manual.",
        DEFAULT_ARG(
            status,
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
    tcontext_pop();
}

void load_plugins(void) {
    // NOTE: the runtime should be fully initalized at the time of calling this!
    ADVANCE_DEFAULT(NULL, load_plugins, PLUGIN_LOAD);
    char* p = cat_path(cm_dir, "plugins");
    if (!exists(p)) create_dir(p);
    else iterate_dir(p, plugin_loader);
    free(p);
    tcontext_pop();
}

void load_middleware(void) {
    cm_middleware = vector_new();
    ADVANCE_DEFAULT(NULL, load_plugins, MIDDLEWARE_LOAD);
    char* p = cat_path(cm_dir, "middleware");
    if (!exists(p)) create_dir(p);
    else iterate_dir(p, middleware_loader);
    free(p);
    tcontext_pop();
}

void iter_commands(map* m, commands_iter_func func) {
    for (int i = 0; i < m->capacity; i++) {
        pair* item = m->items[i];
        if (item) func(
            ((command*) data_content(item->value))->sc
        );
    }
}