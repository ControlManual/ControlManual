#ifndef CM_OBJECT_H
#define CM_OBJECT_H
#include <core/data.h>
#include <core/map.h>
#include <stdbool.h>
#include <core/util.h> // FUNCTYPE
#include <core/vector.h>
#include <stdlib.h> // size_t

typedef struct STRUCT_SCOPE {
    map* global;
    map* local;
} scope;

typedef struct STRUCT_TYPE type;

struct STRUCT_TYPE {
    data* name; // Should be data to char*
    map* attributes; // Everything in this must be data to an object*
    map* cattributes; // May hold any C value (including object*)
    type* parent;
};

typedef struct STRUCT_OBJECT {
    type* tp;
    map* attributes;
    map* cattributes;
} object;

typedef FUNCTYPE(obj_func, object*, (object*, vector*));

extern type base;
extern type integer;
extern type func;

extern object* integer_object;
extern object* func_object;
extern object* boolean_object;

void init_types(void);
void unload_types(void);
extern object* object_get_attr(object* ob, const char* name);
object* object_new(object* tp, vector* params);
object* object_newf(object* tp, size_t len, ...);
object* object_from(type* tp);
type* type_new(
    data* name,
    map* attributes,
    type* parent
);
scope* scope_new(void);
scope* scope_from(map* globals);
void scope_free(scope* s, bool free_globals);
object* object_call_special(object* o, const char* name, vector* args);
bool type_derives(type* src, type* tp);
object* object_internal_new(object* tp, vector* params);
object* object_internal_newf(object* tp, size_t len, ...);
object* object_call(object* o, vector* args);
object* object_callf(object* o, size_t len, ...);

#endif
