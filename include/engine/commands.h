#ifndef CM_COMMAND_H
#define CM_COMMAND_H

#include <core/data.h>
#include <core/util.h>
#include <core/object.h>
#include <core/vector.h>
#include <stdlib.h> // size_t

typedef FUNCTYPE(command_caller, object*, (vector*));

typedef struct STRUCT_PARAM {
    data* name;
    data* description;
    type* tp;
} param;

typedef struct STRUCT_SCHEMA {
    data* name;
    data* description;
    param** params;
    size_t params_len;
} schema;

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

#endif
