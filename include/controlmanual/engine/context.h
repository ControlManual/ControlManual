#ifndef CM_CONTEXT_H
#define CM_CONTEXT_H

#include <controlmanual/core/util.h>
#include <controlmanual/core/vector.h>
#include <controlmanual/core/map.h>
#include <controlmanual/engine/loader.h>
#include <setjmp.h>

typedef struct STRUCT_CONTEXT {
    command* co;
    vector* params;
    vector* flags;
    map* keywords;
    jmp_buf* buf;
} context;

API context* context_new(command* co, vector* params, vector* flags, map* keywords, jmp_buf* buf);
API void context_free(context* c);
API void parse_context(context* c, ...);
API extern void context_abort(context* c);
API void process_context_errors(context* c);
API void missing(context* c, const char* name);

#endif
