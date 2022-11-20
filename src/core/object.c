#include <core/object.h>
#include <core/util.h> // safe_malloc, fail, RETN
#include <stdlib.h> // NULL
#include <core/data.h> // STACK_DATA
#include <core/map.h>
#include <stdbool.h>
#include <string.h> // strlen
#include <stdio.h> // snprintf
#include <core/vector.h>
#include <core/list.h>
#include <core/error.h>
#include <stdarg.h>
#define BUILTIN(tp) map_set( \
    globals, \
    tp.name, \
    CUSTOM_DATA(object_from(&tp), object_dealloc) \
)
#define VA_VEC(vecname, len) va_list args; \
    va_start(args, len); \
    vector* vecname = vector_new(); \
    for (int i = 0; i < len; i++) \
        vector_append(v, va_arg(args, data*)); \
    va_end(args)
#define SETATTR(obj, name) map_set( \
        obj.cattributes, \
        STACK_DATA(#name), \
        STACK_DATA(obj## name##) \
    )
#define LOADBUILTIN(nm, str) nm.name = STACK_DATA(str); \
    nm.attributes = map_new(1); \
    nm.parent = &base; \
    nm.cattributes = map_new(4);
#define IARG(tp) { \
                tp* ptr = va_arg(args, tp*); \
                *ptr = obj; \
                break; \
            }
#define ARG(tp) { \
    object** ptr = va_arg(args, object**); \
    if (!ensure_derives(obj, &tp)) return false; \
    *ptr = obj; \
    break; \
}
#define BUILDOBJ(nm) nm##_object = object_from(&nm);

/* Argument parsing */

static bool ensure_derives(object* ob_a, type* tp) {
    if (!type_derives(ob_a->tp, tp)) {
        char* tp_name = data_content(tp->name);
        char* ob_name = data_content(ob_a->tp->name);
        size_t size = strlen(tp_name) + strlen(ob_name) + 16;

        char* str = safe_malloc(size);
        snprintf(str, size, "expected %s, got %s", tp_name, ob_name);
        THROW_HEAP(str);
        return false;
    }

    return true;
}

bool parse_args(vector* params, const char* format, ...) {
    va_list args;
    va_start(args, format);
    size_t len = strlen(format);

    for (int i = 0; i < len; i++) {
        char c = format[i];
        object* obj = vector_get(params, i);
        if (!obj) {
            THROW_STATIC("not enough arguments");
            return false;
        }

        switch (c) {
            case 'i': ARG(integer);
        }
    }

    va_end(args);
    return true;
}

void parse_iargs(vector* params, const char* format, ...) {
    va_list args;
    va_start(args, format);
    size_t len = strlen(format);

    for (int i = 0; i < len; i++) {
        char c = format[i];
        void* obj = vector_get(params, i);
        if (!obj) fail("not enough arguments");

        switch (c) {
            case 'i': IARG(int*)
            case 'f': IARG(obj_func)
            case 'v': IARG(vector*)
            case 'm': IARG(map*)
        }
    }

    va_end(args);
}

/* Builtin object methods */

static object* func_call(object* o, vector* args) {
    
}

static object* func_construct(object* o, vector* args) {
    SET_CATTR_SS(o, _caller, func_call);
}

static object* func_iconstruct(object* o, vector* args) {
    obj_func f;
    parse_iargs(args, "f", &f);
    SET_CATTR_SS(o, _caller, f);
}

static object* integer_iconstruct(object* o, vector* args) {
    int value;
    parse_iargs(args, "i", &value);
    int* heap_value = safe_malloc(sizeof(int));
    *heap_value = value;
    SET_CATTR_SH(o, value, heap_value);
}

static object* integer_construct(object* o, vector* args) {
    object* value;
    if (!parse_args(args, "i", &value)) return NULL;
    SET_ATTR_SH(o, value, value);
}

static object* array_iconstruct(object* o, vector* args) {
    vector* v;
    parse_iargs(args, "v", &v);
    SET_CATTR_SH(o, value, v);
}

static object* hashmap_iconstruct(object* o, vector* args) {
    map* m;
    parse_iargs(args, "m", &m);
    SET_CATTR_SH(o, value, m);
}

/* Builtin types and objects */

type base;
type integer;
type func;
type boolean;
type array;
type hashmap;

object* integer_object;
object* func_object;
object* boolean_object;
object* array_object;
object* hashmap_object;

/* Type functions */

/* Initalize all types. */
void init_types(void) {
    map* m = map_new(8);
    map_set(m, STACK_DATA("a"), STACK_DATA("b"));

    base.name = STACK_DATA("base");
    base.attributes = m;
    base.cattributes = map_new(4);
    base.parent = &base;

    LOADBUILTIN(integer, "int");
    LOADBUILTIN(func, "function");
    LOADBUILTIN(boolean, "bool");
    LOADBUILTIN(array, "array");
    LOADBUILTIN(hashmap, "hashmap");

    //SETATTR(func, _construct);
    //SETATTR(func, _iconstruct);
    //SETATTR(integer, _iconstruct);
    //SETATTR(array, _iconstruct);
    //SETATTR(hashmap, _iconstruct);

    BUILDOBJ(func);
    BUILDOBJ(integer);
    BUILDOBJ(boolean);
    BUILDOBJ(array);
    BUILDOBJ(hashmap);
}

/* Free builtin types. */
void unload_types(void) {
    // unfinished
    map_free(integer.attributes);
    map_free(func.attributes);
    map_free(base.attributes);
}

/* Does the source type derive from the target. */
bool type_derives(type* src, type* tp) {
    type* current = src;
    while (current != &base) {
        if (current == tp) return true;
        current = current->parent;
    }
    
    return false;
}

/* New type object. */
type* type_new(
    data* name,
    map* attributes,
    type* parent
) {
    type* tp = safe_malloc(sizeof(type));
    tp->name = name;
    tp->attributes = attributes;
    tp->parent = parent;
    return tp;
}

/* Object functions */

/* Get Control Manual object at the attribute. NULL if not found. */
inline object* object_get_attr(object* ob, const char* name) {
    return map_get(ob->attributes, name);
}

/* Get the C object at the attribute. NULL if not found. */
void* object_get_cattr(object* ob, const char* name) {
    return map_get(ob->cattributes, name);
}

static object* object_alloc(object* tp) {
    object* obj = safe_malloc(sizeof(object));
    obj->attributes = map_copy(tp->attributes);
    obj->cattributes = map_copy(tp->cattributes);
    obj->tp = tp->tp;

    return obj;
}

/*
Instantiate a new object from a type.
 
Everything in params object should be a Control Manual object.
*/
object* object_new(object* tp, vector* params) {
    object* obj = object_alloc(tp);
    if (!object_call_special(obj, "_construct", params)) return NULL;

    return obj;
}

/* New object from a format string. len should match the number of arguments. */
object* object_newf(object* tp, size_t len, ...) {
    VA_VEC(v, len);
    object* obj = object_new(tp, v);
    vector_free(v);

    return obj;
}

/* Create a new object using C value opposed to a Control Manual object. */
object* object_internal_new(object* tp, vector* params) {
    object* obj = object_alloc(tp);
    if (!object_call_special(obj, "_iconstruct", params)) return NULL;

    return obj;
}

/* Create a new internal object from a format string. len should match the number of argument. */
object* object_internal_newf(object* tp, size_t len, ...) {
    VA_VEC(v, len);
    object* obj = object_internal_new(tp, v);
    vector_free(v);

    return obj;
}

/* Create an object from a type. NOT INSTANTIATING. */
object* object_from(type* tp) {
    object* o = safe_malloc(sizeof(object));
    o->attributes = map_copy(tp->attributes);
    o->cattributes = map_copy(tp->cattributes);
    o->tp = tp;
    return o;
}

/* Call the specified object. Everything in args should be a Control Manual object. */
object* object_call(object* o, vector* args) {
    obj_func func = CATTR(o, _caller);
    if (!func) RETN(THROW_STATIC("object is not callable"))
    vector* v = args ? args : vector_new();
    object* res = func(o, v);
    if (!args) vector_free(v);

    return res;
}

/* Call an object with a format string. len should match the number of arguments. */
object* object_callf(object* o, size_t len, ...) {
    VA_VEC(v, len);
    object* obj = object_call(o, v);
    vector_free(v);
    return obj;
}

/* Call a special method on the object. */
object* object_call_special(object* o, const char* name, vector* args) {
    obj_func cfunc = object_get_cattr(o, name);
    if (cfunc) {
        vector* v = args ? args : vector_new();
        object* result = cfunc(o, v);
        if (!args) vector_free(v);
        return result;
    }

    object* attr = object_get_attr(o, name);
    if (!attr) {
        size_t size = strlen(name) + 26;
        char* str = safe_malloc(size);
        snprintf(str, size, "%s does not exist on object", name);
        THROW_HEAP(str);
        return NULL;
    }

    if (VECTOR_LENGTH(args)) vector_insert(args, 0, NOFREE_DATA(o));
    else vector_append(args, NOFREE_DATA(o));
    return object_call(attr, args);
}

static void object_dealloc(object* obj) {
    map_free(obj->attributes);
    map_free(obj->cattributes);
    free(obj);
}

/* Scope functions */

/* Create a new scope */
scope* scope_new(void) {
    scope* s = safe_malloc(sizeof(scope));
    map* globals = map_new(16);
    s->global = globals;
    s->local = map_new(8);

    BUILTIN(base);
    BUILTIN(integer);
    BUILTIN(func);
    // unfinished
}

/* Create a new scope using an existing global map. */
scope* scope_from(map* globals) {
    scope* s = safe_malloc(sizeof(scope));
    s->local = map_new(8);
    s->global = globals;
    return s;
}

/* Free a scope (and its objects). */
void scope_free(scope* s, bool free_globals) {
    map_free(s->local);
    if (free_globals) map_free(s->global);
    free(s);
}

/* Utilities */

/* Integer from a C integer. */
object* integer_from(int value) {
    int* ptr = safe_malloc(sizeof(int));
    *ptr = 1;
    return object_internal_newf(integer_object, 1, HEAP_DATA(ptr));
}

/* Function from a C function. */
object* func_from(obj_func function) {
    return object_internal_newf(func_object, 1, STACK_DATA(function));
}