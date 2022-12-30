#ifndef CM_COMMAND_H
#define CM_COMMAND_H

#include <controlmanual/core/data.h>
#include <controlmanual/core/util.h>
#include <controlmanual/core/object.h>
#include <controlmanual/core/vector.h>
#include <stdlib.h> // size_t
#include <stdbool.h>

#define NUMARGS(...)  (sizeof((param*[]) { __VA_ARGS__ }) / sizeof(param*))

typedef struct STRUCT_PARAM {
    data* name;
    data* description;
    bool flag;
    bool keyword;
    bool required;
    data* df;
    type* tp;
    bool convert;
    data* shorthand;
} param;

typedef struct STRUCT_CONTEXT context;

typedef struct STRUCT_SCHEMA {
    data* name;
    data* description;
    param** params;
    size_t params_len;
} schema;

typedef FUNCTYPE(command_caller, object*, (context*));

typedef struct STRUCT_COMMAND {
    schema* sc;
    command_caller caller;
} command;

schema* schema_new(data* name, data* description, param** params, size_t params_len);
command* command_new(command_caller caller, schema* sc);
void load_commands(void);

extern map* commands;

typedef FUNCTYPE(commands_iter_func, void, (schema*));
void iter_commands(map* m, commands_iter_func func);

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
);
param** param_array_from(param** array, size_t size);

extern type cm_any_wrapper;

#define any cm_any_wrapper

#define EXPR(value) #value
#define RAW_FLAG(name, shorthand, desc, convert) param_new(STACK_DATA(name), STACK_DATA(desc), NULL, true, true, false, NULL, convert, STACK_DATA(shorthand))
#define RAW_KWARG(name, shorthand, desc, tp, default, convert) param_new(STACK_DATA(name), STACK_DATA(desc), &tp, false, true, false, STACK_DATA(EXPR(default)), convert, STACK_DATA(shorthand))
#define RAW_ARG(name, desc, tp, convert) param_new(STACK_DATA(name), STACK_DATA(desc), &tp, false, false, true, NULL, convert, NULL)
#define RAW_DEFAULT_ARG(name, desc, tp, expr, convert) param_new(STACK_DATA(name), STACK_DATA(desc), &tp, false, false, false, STACK_DATA(EXPR(expr)), convert, NULL)

#define FLAG(name, shorthand, desc) RAW_FLAG(name, shorthand, desc, true)
#define KWARG(name, shorthand, desc, tp, default) RAW_KWARG(name, shorthand, desc, tp, default, true)
#define ARG(name, desc, tp) RAW_ARG(name, desc, tp, true)
#define DEFAULT_ARG(name, desc, tp, expr) RAW_DEFAULT_ARG(name, desc, tp, expr, true)

#define FLAG_NOCONV(name, shorthand, desc) RAW_FLAG(name, shorthand, desc, false)
#define KWARG_NOCONV(name, shorthand, desc, tp, default) RAW_KWARG(name, shorthand, desc, tp, default, false)
#define ARG_NOCONV(name, desc, tp) RAW_ARG(name, desc, tp, false)
#define DEFAULT_ARG_NOCONV(name, desc, tp, expr) RAW_DEFAULT_ARG(name, desc, tp, expr, false)

typedef struct STRUCT_PARAMCONTEXT {
    param** params;
    size_t len;
} paramcontext;

paramcontext* paramcontext_new(param** params, size_t len);

typedef FUNCTYPE(dir_iter_func, void, (char*));
typedef FUNCTYPE(param_construct_func, paramcontext*, ());
typedef FUNCTYPE(command_caller_func, object*, (context*));
typedef FUNCTYPE(get_str_func, data*, ());

#endif
