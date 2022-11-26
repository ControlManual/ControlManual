#include <core/object.h>
#include <core/util.h> // safe_malloc, FAIL, RETN, int_convert
#include <stdlib.h> // NULL
#include <core/data.h> // STACK_DATA
#include <core/map.h>
#include <stdbool.h>
#include <string.h> // strlen
#include <stdio.h> // snprintf, sprintf
#include <core/vector.h>
#include <core/list.h>
#include <core/error.h>
#include <stdarg.h>
#include <math.h> // log10
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
    nm.cattributes = map_new(1); \
    nm.construct = nm##_construct; \
    nm.iconstruct = nm##_iconstruct; \
    nm.call = base_call; \
    nm.to_string = nm##_to_string; \
    BUILDOBJ(nm);
#define IARG(tp) { \
                tp* ptr = va_arg(args, tp*); \
                *ptr = obj; \
                break; \
            }
#define IARG_DRF(tp) { \
                tp* ptr = va_arg(args, tp*); \
                *ptr = *(tp*) obj; \
                break; \
            }
#define ARG(tp) { \
    object** ptr = va_arg(args, object**); \
    if (!ensure_derives(obj, &tp)) return false; \
    *ptr = obj; \
    break; \
}

#define ARGCON(tp, ctp) { \
    ctp* ptr = va_arg(args, ctp*); \
    if (!ensure_derives(obj, &tp)) return false; \
    *ptr = obj->value; \
    break; \
}
#define ARGCON_DRF(tp, ctp) { \
    ctp* ptr = va_arg(args, ctp*); \
    if (!ensure_derives(obj, &tp)) return false; \
    *ptr = *((ctp*) obj->value); \
    break; \
}
#define BUILDOBJ(nm) nm##_object = object_from(&nm);
#define CONSTRUCT_SIMPLE(nm, fmtc) static void nm##_construct(object* o, vector* args) { \
    object* value; \
    if (!parse_args(args, fmtc, &value)) return; \
    o->value = value; \
}
#define ICONSTRUCT_SIMPLE(nm, tp, fmtc) static void nm##_iconstruct(object* o, vector* args) { \
    tp value; \
    parse_iargs(args, fmtc, &value); \
    o->value = value; \
}
#define ICONSTRUCT_SIMPLE_CON(nm, tp, fmtc) static void nm##_iconstruct(object* o, vector* args) { \
    tp value; \
    parse_iargs(args, fmtc, &value); \
    o->value = int_convert((int) value); \
}

/* Argument parsing */

static bool ensure_derives(object* ob_a, type* tp) {
    if (!type_derives(ob_a->tp, tp)) {
        char* tp_name = data_content(tp->name);
        char* ob_name = data_content(ob_a->tp->name);
        size_t size = strlen(tp_name) + strlen(ob_name) + 16;

        char* str = safe_malloc(size);
        snprintf(str, size, "expected %s, got %s", tp_name, ob_name);
        THROW_HEAP(str, "<arguments>");
        return false;
    }

    return true;
}

bool parse_args(vector* params, const char* format, ...) {
    va_list args;
    va_start(args, format);
    size_t len = strlen(format);
    bool optional = false;
    size_t index = 0;

    for (int i = 0; i < len; i++) {
        char c = format[i];

        if (c == '|') {
            optional = true;
            continue;
        }
        

        object* obj = vector_get(params, index);
        if (!obj) {
            if (!optional) THROW_STATIC("not enough arguments", "<arguments>");
            return optional;
        }

        switch (c) {
            case 'I': ARG(integer);
            case 'i': ARGCON_DRF(integer, int);
            case 'S': ARG(string);
            case 's': ARGCON(string, char*);
            case 'B': ARG(boolean);
            case 'b': ARGCON_DRF(boolean, bool);
        };
        index++;
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
        if (!obj) FAIL("not enough arguments");

        switch (c) {
            case 'i': IARG_DRF(int)
            case 'f': IARG(obj_func)
            case 'v': IARG(vector*)
            case 'm': IARG(map*)
            case 'b': IARG_DRF(bool)
            case 's': IARG(char*)
        }
    }

    va_end(args);
}

/* Builtin object methods */

object* base_call(object* o, vector* args) {
    char* content = data_content(o->tp->name);
    char* str = safe_malloc(16 + (strlen(content)));
    sprintf(str, "%s is not callable", content);
    THROW_HEAP(str, "<calling>");
}

inline object* base_dealloc(object* o) {
    map_free(o->attributes);
    map_free(o->cattributes);
    free(o);
}

object* base_to_string(object* o) {
    char* content = data_content(o->tp->name);
    char* str = safe_malloc(10 + strlen(content));
    sprintf(str, "[%s object]", content);
    return string_from(HEAP_DATA(str));
}

static void base_construct(object* o, vector* args) {
    THROW_STATIC("base is not constructable", "<constructing>");
}

static void base_iconstruct(object* o, vector* args) {
    FAIL("base is not constructable");
}


static object* func_call(object* o, vector* args) {
    
}

static void func_construct(object* o, vector* args) {
    
}

ICONSTRUCT_SIMPLE(func, obj_func, "f")
ICONSTRUCT_SIMPLE_CON(integer, int, "i")
CONSTRUCT_SIMPLE(integer, "i")
CONSTRUCT_SIMPLE(boolean, "b")

static void string_construct(object* o, vector* args) {
    object* ob = vector_get(args, 0);
    if (!ob) {
        THROW_STATIC("not enough arguments", "<arguments>");
        return;
    }
    o->value = o->tp->to_string(o);
}

static object* string_to_string(object* o) {
    return o;
}

static object* boolean_to_string(object* o) {
    return string_from(STACK_DATA(o->value ? "true" : "false"));
}

static object* integer_to_string(object* o) {
    char* str = safe_malloc(sizeof(char) * (int) log10(*((int*) o->value)));
    // ^^ i dont know what the hell the log10 thing does, i stole it from stack overflow
    sprintf(str, "%d", o->value);
    return string_from(HEAP_DATA(strdup(o->value)));
}

static object* func_to_string(object* o) {
    return string_from(STACK_DATA("[function]"));
}

ICONSTRUCT_SIMPLE(string, char*, "s")
ICONSTRUCT_SIMPLE_CON(boolean, bool, "b")

/* Builtin types and objects */

type base;
type integer;
type func;
type string;
type boolean;

object* integer_object;
object* func_object;
object* boolean_object;
object* string_object;
object* base_object;

/* Type functions */

/* Initalize all types. */
void init_types(void) {
    LOADBUILTIN(base, "base");
    LOADBUILTIN(integer, "int");
    LOADBUILTIN(func, "function");
    func.call = func_call;
    LOADBUILTIN(boolean, "bool");
    LOADBUILTIN(string, "str");
}

/* Free builtin types. */
void unload_types(void) {
    
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
    type* parent,
    obj_func_noret iconstruct,
    obj_func_noret construct,
    obj_func call,
    obj_func_noargs to_string,
    obj_func_noargs dealloc
) {
    type* tp = safe_malloc(sizeof(type));
    tp->name = name;
    tp->attributes = attributes ? attributes : map_copy(parent->attributes);
    tp->parent = parent;
    tp->iconstruct = iconstruct ? iconstruct : parent->iconstruct;
    tp->construct = construct ? construct : parent->construct;
    tp->call = call ? call : parent->call;
    tp->to_string = to_string ? to_string : parent->to_string;
    tp->dealloc = dealloc ? dealloc : parent->dealloc;
    return tp;
}

/* Object functions */

static object* object_alloc(object* tp) {
    object* obj = safe_malloc(sizeof(object));
    obj->attributes = map_copy(tp->attributes);
    obj->cattributes = map_copy(tp->cattributes);
    obj->tp = tp->tp;
    obj->value = NULL;

    return obj;
}

/*
Instantiate a new object from a type.
 
Everything in params object should be a Control Manual object.
*/
object* object_new(object* tp, vector* params) {
    object* obj = object_alloc(tp);
    obj->tp->construct(obj, params);

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
    obj->tp->iconstruct(obj, params);

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
    if (!func) RETN(THROW_STATIC("object is not callable", "<calling>"))
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
    return object_internal_newf(integer_object, 1, HEAP_DATA(int_convert(value)));
}

/* Function from a C function. */
object* func_from(obj_func function) {
    return object_internal_newf(func_object, 1, STACK_DATA(function));
}

object* string_from(data* value) {
    return object_internal_newf(string_object, 1, value);
}
