#ifndef CM_CONTEXT_H
#define CM_CONTEXT_H

#include <controlmanual/engine/commands.h>
#include <controlmanual/core/vector.h>
#include <controlmanual/core/map.h>

typedef struct STRUCT_CONTEXT {
    command* co;
    vector* params;
    vector* flags;
    map* keywords;
} context;

context* context_new(command* co, vector* params, vector* flags, map* keywords);
void context_free(context* c);
bool parse_context(context* c, ...);

#endif