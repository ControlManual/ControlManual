#include <core/object.h>
#include <core/util.h> // safe_malloc, fail
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
#define SETATTR(obj, k, v) map_set( \
        obj.cattributes, \
        STACK_DATA(k), \
        STACK_DATA(v) \
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
#define BUILDOBJ(nm) nm##_object = object_from(&nm);

type base;
type integer;
type func;
type boolean;

object* integer_object;
object* func_object;
object* boolean_object;

static bool ensure_derives(object* ob_a, type* tp) {
    if (!type_derives(ob_a->tp, tp)) {
        char* tp_name = data_content(tp->name);
        char* ob_name = data_content(ob_a->tp->name);
        size_t size = strlen(tp_name) + strlen(ob_name) + 16;

        char* str = safe_malloc(size);
        snprintf(str, size, "expected %s, got %s", tp_name, ob_name);
        throw(HEAP_DATA(str), STACK_DATA("<native code>"), NULL);
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
            case 'i':
                object** ptr = va_arg(args, object**);
                if (!ensure_derives(obj, &integer)) return false;
                *ptr = obj;
                break;
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
        }
    }

    va_end(args);
}

static object* func_call(object* ob, vector* args) {
    
}

static object* func_construct(object* ob, vector* args) {
    map_set(ob->cattributes, STACK_DATA("_caller"), STACK_DATA(func_call));
}

static object* func_iconstruct(object* ob, vector* args) {
    obj_func f;
    parse_iargs(args, "f", &f);
    map_set(ob->cattributes, STACK_DATA("_caller"), STACK_DATA(f));
}

static object* int_iconstruct(object* o, vector* args) {
    int value;
    parse_iargs(args, "i", &value);
    int* heap_value = safe_malloc(sizeof(int));
    *heap_value = value;
    map_set(o->cattributes, STACK_DATA("value"), HEAP_DATA(heap_value));
}

static object* int_construct(object* o, vector* args) {
    object* value;
    if (!parse_args(args, "i", &value)) return NULL;
    map_set(o->attributes, STACK_DATA("value"), HEAP_DATA(value));
}

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

    SETATTR(func, "_construct", func_construct);
    SETATTR(func, "_iconstruct", func_iconstruct);
    SETATTR(integer, "_iconstruct", int_iconstruct);

    BUILDOBJ(func);
    BUILDOBJ(integer);
    BUILDOBJ(boolean);

}

void unload_types(void) {
    map_free(integer.attributes);
    map_free(func.attributes);
    map_free(base.attributes);
}

inline object* object_get_attr(object* ob, const char* name) {
    return map_get(ob->attributes, name);
}

void* object_get_cattr(object* ob, const char* name) {
    return map_get(ob->cattributes, name);
}

bool type_derives(type* src, type* tp) {
    type* current = src;
    while (current != &base) {
        if (current == tp) return true;
        current = current->parent;
    }
    
    return false;
}

static object* object_alloc(object* tp) {
    object* obj = safe_malloc(sizeof(object));
    obj->attributes = map_copy(tp->attributes);
    obj->cattributes = map_copy(tp->cattributes);
    obj->tp = tp->tp;

    return obj;
}
/* Instantiate a new object from a type. */
object* object_new(object* tp, vector* params) {
    object* obj = object_alloc(tp);
    if (!object_call_special(obj, "_construct", params)) return NULL;

    return obj;
}

object* object_newf(object* tp, size_t len, ...) {
    VA_VEC(v, len);
    object* obj = object_new(tp, v);
    vector_free(v);

    return obj;
}

object* object_internal_new(object* tp, vector* params) {
    object* obj = object_alloc(tp);
    if (!object_call_special(obj, "_iconstruct", params)) return NULL;

    return obj;
}

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

object* object_call(object* o, vector* args) {
    if (!type_derives(o->tp, &func)) {
        THROW_STATIC("doesnt derive from func");
        return NULL;
    }
    
    obj_func func = object_get_cattr(o, "_caller");
    vector* v = args ? args : vector_new();
    object* res = func(o, v);
    if (!args) vector_free(v);

    return res;
}

object* object_callf(object* o, size_t len, ...) {
    VA_VEC(v, len);
    object* obj = object_call(o, v);
    vector_free(v);
    return obj;
}

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

    if (args->size) vector_insert(args, 0, NOFREE_DATA(o));
    else vector_append(args, NOFREE_DATA(o));
    return object_call(attr, args);
}

static void object_dealloc(object* obj) {
    map_free(obj->attributes);
    map_free(obj->cattributes);
    free(obj);
}

scope* scope_new(void) {
    scope* s = safe_malloc(sizeof(scope));
    map* globals = map_new(16);
    s->global = globals;
    s->local = map_new(8);

    BUILTIN(base);
    BUILTIN(integer);
    BUILTIN(func);
}

scope* scope_from(map* globals) {
    scope* s = safe_malloc(sizeof(scope));
    s->local = map_new(8);
    s->global = globals;
    return s;
}

void scope_free(scope* s, bool free_globals) {
    map_free(s->local);
    if (free_globals) map_free(s->global);
    free(s);
}