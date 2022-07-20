#ifndef __CM_SESSION_H__
#define __CM_SESSION_H__

#include <utilities.h>

/* Structure containing data for a scope. */
typedef struct STRUCT_SCOPE scope;

struct STRUCT_SCOPE {
    map* objects;
    scope* global;
};
void scope_free(scope* sc);
void scope_add_type(scope* sc, type_object* type);
scope* scope_new(scope* global);

#endif
