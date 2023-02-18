#ifndef CM_OBJECT_H
#define CM_OBJECT_H
#include <controlmanual/core/data.h>
#include <controlmanual/core/map.h>
#include <controlmanual/core/util.h> // FUNCTYPE
#include <controlmanual/core/vector.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdlib.h> // size_t

/* All object attributes should use the following macros incase of an API change in the future. */

#define SET_ATTR(o, k, v) map_set(o->attributes, k, v)

#define SET_ATTR_S_SH(o, k, v) SET_ATTR(o, STACK_DATA(k), HEAP_DATA(v))
#define SET_ATTR_S_SS(o, k, v) SET_ATTR(o, STACK_DATA(k), STACK_DATA(v))
#define SET_ATTR_SH(o, k, v) SET_ATTR_S_SH(o, #k, v)
#define SET_ATTR_SS(o, k, v) SET_ATTR_S_SS(o, #k, v)
#define SET_ATTR_HS(o, k, v) SET_ATTR(o, HEAP_DATA(k), STACK_DATA(v))
#define SET_ATTR_HH(o, k, v) SET_ATTR(o, HEAP_DATA(k), HEAP_DATA(v))

#define ATTR_S(o, k) map_get(o->attributes, k)
#define ATTR(o, k) map_get(o->attributes, #k)

#define STRING_VALUE(o) ((char*) data_content(o->value))
#define TYPE(o) ((type*) o->tp)

#define OBJECT_DATA(o) CUSTOM_DATA(o, object_dealloc)

typedef struct STRUCT_SCOPE {
    map* global;
    map* local;
} scope;

typedef struct STRUCT_TYPE type;
typedef struct STRUCT_OBJECT object;

typedef FUNCTYPE(obj_func, object*, (object*, vector*));
typedef FUNCTYPE(obj_func_noret, void, (object*, vector*));
typedef FUNCTYPE(obj_func_noargs, object*, (object*));
typedef FUNCTYPE(obj_dealloc, void, (object*));

struct STRUCT_TYPE {
    type* parent;
    obj_func_noret iconstruct;
    obj_func_noret construct;
    obj_func call;
    obj_func_noargs to_string;
    obj_dealloc dealloc;
    obj_func_noargs iter;
    data* name;
    map* methods;
};

struct STRUCT_OBJECT {
    type* tp;
    map* attributes;
    void* value;
};

extern type base;
extern type integer;
extern type func;
extern type string;
extern type boolean;
extern type array;
extern type iterator;

extern object* command_exec(const char* str);
// we really shouldn't try and access the engine from the core, but we have to make an exception here


API void init_types(void);
API extern object* object_get_attr(object* ob, const char* name);
API object* object_new(object* tp, vector* params);
API object* object_newf(object* tp, size_t len, ...);
API object* object_from(data* tp);
API scope* scope_new(void);
API scope* scope_from(map* globals);
API void scope_free(scope* s, bool free_globals);
API bool type_derives(type* src, type* tp);
API object* object_internal_new(object* tp, vector* params);
API object* object_internal_newf(object* tp, size_t len, ...);
API object* object_call(object* o, vector* args);
API object* object_callf(object* o, size_t len, ...);
API object* string_from(data* str);
API object* integer_from(int value);
API object* scope_get(scope* s, char* name);

API bool parse_vargs(vector* params, const char* format, va_list args);
API bool parse_args(vector* params, const char* format, ...);
API bool parse_args_meth(vector* params, type* self, const char* format, ...);
API bool ensure_derives(object* ob_a, type* tp);
API bool object_compare(object* a, object* b);
API bool type_compare(type* a, type* b);
API object* iterator_from(vector* values);
API object* array_from(vector* value);
API extern void object_dealloc(object* ob);
API object* string_fmt(const char* fmt, ...) ;

#define GLOBAL cm_global_scope
extern scope* cm_global_scope;

API object* func_from(obj_func func);
API object* object_to_string(object* o);

#endif
