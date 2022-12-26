#ifndef CM_COMMAND_H
#define CM_COMMAND_H

#include <core/data.h>
#include <core/util.h>
#include <core/object.h>
#include <core/vector.h>
#include <stdlib.h> // size_t
#include <stdbool.h>

typedef struct STRUCT_PARAM {
    data* name;
    data* description;
    bool flag;
    bool keyword;
    bool required;
    data* df;
    type* tp;
    bool convert;
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
    bool convert
);
param** param_array_from(param** array, size_t size);

extern type cm_any_wrapper;

#define any cm_any_wrapper

#define EXPR(value) #value
#define RAW_FLAG(name, desc, convert) param_new(STACK_DATA(name), STACK_DATA(desc), NULL, true, true, false, NULL, convert)
#define RAW_KWARG(name, desc, tp, default, convert) param_new(STACK_DATA(name), STACK_DATA(desc), &tp, false, true, false, STACK_DATA(EXPR(KWARG)), convert)
#define RAW_ARG(name, desc, tp, convert) param_new(STACK_DATA(name), STACK_DATA(desc), &tp, false, false, true, NULL, convert)
#define RAW_DEFAULT_ARG(name, desc, tp, expr, convert) param_new(STACK_DATA(name), STACK_DATA(desc), &tp, false, false, false, STACK_DATA(EXPR(expr)), convert)

#define FLAG(name, desc) RAW_FLAG(name, desc, true)
#define KWARG(name, desc, tp, default) RAW_KWARG(name, desc, tp, default, true)
#define ARG(name, desc, tp) RAW_ARG(name, desc, tp, true)
#define DEFAULT_ARG(name, desc, tp, expr) RAW_DEFAULT_ARG(name, desc, tp, expr, true)

#define FLAG_NOCONV(name, desc) RAW_FLAG(name, desc, false)
#define KWARG_NOCONV(name, desc, tp, default) RAW_KWARG(name, desc, tp, default, false)
#define ARG_NOCONV(name, desc, tp) RAW_ARG(name, desc, tp, false)
#define DEFAULT_ARG_NOCONV(name, desc, tp, expr) RAW_DEFAULT_ARG(name, desc, tp, expr, false)


#endif
