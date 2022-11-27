#ifndef CM_CONTEXT_H
#define CM_CONTEXT_H

#include <engine/commands.h>
#include <core/vector.h>
#include <core/map.h>

typedef struct STRUCT_CONTEXT {
    command* co;
    vector* params;
    vector* flags;
    map* keywords;
} context;

context* context_new(command* co, vector* params, vector* flags, map* keywords);
void context_free(context* c);

#endif