#ifndef __CM_SESSION_H__
#define __CM_SESSION_H__

#include <utilities.h>

typedef struct STRUCT_SESSION session;

/* Structure containing data for a scope. */
typedef struct STRUCT_SCOPE {
    map* types;
    map* instances;
    session* parent;
} scope;

struct STRUCT_SESSION {
    scope* global;
    vector* scopes;
};

#endif
