#ifndef CM_COMMAND_H
#define CM_COMMAND_H

#include <core/data.h>
#include <core/util.h>
#include <core/object.h>
#include <core/vector.h>

typedef struct STRUCT_SCHEMA {
    data* name;
    data* description;
    vector* params;
} schema;

typedef struct STRUCT_COMMAND {
    FUNCTYPE(caller, object*, (vector*));
    data* schema;
} command;

#endif