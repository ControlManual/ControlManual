#ifndef CM_OBJECT_H
#define CM_OBJECT_H
#include <core/data.h>
#include <core/map.h>
#include <stdbool.h>
#include <core/util.h> // FUNCTYPE
#include <core/vector.h>
#include <stdlib.h> // size_t

/* All object attributes should use the following macros incase of an API change in the future. */

#define SET_ATTR(o, k, v) map_set(o->attributes, k, v)
#define SET_CATTR(o, k, v) map_set(o->cattributes, k, v)

#define SET_ATTR_S_SH(o, k, v) SET_ATTR(o, STACK_DATA(k), HEAP_DATA(v))
#define SET_ATTR_S_SS(o, k, v) SET_ATTR(o, STACK_DATA(k), STACK_DATA(v))
#define SET_ATTR_SH(o, k, v) SET_ATTR_S_SH(o, #k, v)
#define SET_ATTR_SS(o, k, v) SET_ATTR_S_SS(o, #k, v)
#define SET_ATTR_HS(o, k, v) SET_ATTR(o, HEAP_DATA(k), STACK_DATA(v))
#define SET_ATTR_HH(o, k, v) SET_ATTR(o, HEAP_DATA(k), HEAP_DATA(v))

#define SET_CATTR_S_SH(o, k, v) SET_CATTR(o, STACK_DATA(k), HEAP_DATA(v))
#define SET_CATTR_S_SS(o, k, v) SET_CATTR(o, STACK_DATA(k), STACK_DATA(v))
#define SET_CATTR_SS(o, k, v) SET_CATTR_S_SS(o, #k, v)
#define SET_CATTR_SH(o, k, v) SET_CATTR_S_SH(o, #k, v)
#define SET_CATTR_HS(o, k, v) SET_CATTR(o, HEAP_DATA(k), STACK_DATA(v))
#define SET_CATTR_HH(o, k, v) SET_CATTR(o, HEAP_DATA(k), HEAP_DATA(v))

#define ATTR_S(o, k) map_get(o->attributes, k)
#define ATTR(o, k) map_get(o->attributes, #k)

#define CATTR_S(o, k) map_get(o->cattributes, k)
#define CATTR(o, k) map_get(o->cattributes, #k)

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