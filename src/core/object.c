#include <controlmanual/core/data.h> // STACK_DATA
#include <controlmanual/core/map.h>
#include <controlmanual/core/vector.h>
#include <controlmanual/core/list.h>
#include <controlmanual/core/error.h>
#include <controlmanual/core/object.h>
#include <controlmanual/core/util.h> // safe_malloc, FAIL, RETN, int_convert
#include <stdlib.h> // NULL
#include <stdbool.h>
#include <string.h> // strlen
#include <stdio.h> // snprintf, sprintf
#include <stdarg.h>
#include <math.h> // log10
#define BUILTIN(tp) map_set( \
    globals, \
    tp.name, \
    OBJECT_DATA(object_from(STACK_DATA(&tp))) \
)
#define VA_VEC(vecname, len) va_list args; \
    va_start(args, len); \
    vector* vecname = vector_new(); \
    for (int i = 0; i < len; i++) \
        vector_append(v, va_arg(args, data*)); \
    va_end(args)
#define SETATTR(obj, name) map_set( \
        STACK_DATA(#name), \
        STACK_DATA(obj## name##) \
    )
#define LOADBUILTIN(nm, str) nm.name = STACK_DATA(str); \
    nm.attributes = map_new(1); \
    nm.parent = &base; \
    nm.construct = nm##_construct; \
    nm.iconstruct = nm##_iconstruct; \
    nm.call = base_call; \
    nm.to_string = nm##_to_string; \
    nm.iter = nm##_iter; \
    nm.dealloc = nm##_dealloc;
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
#define ITER_SIMPLE(nm) static object* nm##_iter(object* o) { THROW_STATIC("object is not iterable", "<iterating>"); return NULL; }
#define GETOBJ(tp) ((object*) scope_get(GLOBAL, data_content(tp.name)))

scope* GLOBAL = NULL;


/* Argument parsing */

bool ensure_derives(object* ob, type* tp) {
    if (!type_derives(ob->tp, tp)) {
        char* tp_name = data_content(tp->name);
        puts(STRING_VALUE(ob->tp->to_string(ob)));
        char* ob_name = data_content(ob->tp->name);
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
            case 'a': {
                object** ptr = va_arg(args, object**);
                *ptr = obj;
                break;
            }
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
            case 'd': IARG(data*)
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
    return NULL;
}

void base_dealloc(object* o) {
    type* tp = data_content(o->value);
    //data_free(tp->name);
    map_free(tp->attributes);
    data_free(o->value);
    map_free(o->attributes);
    free(o);
}

object* base_to_string(object* o) {
    char* content = data_content(((type*) data_content(o->value))->name);
    char* str = safe_malloc(8 + strlen(content));
    sprintf(str, "[%s base]", content);
    return string_from(NOFREE_DATA(str));
}

static void base_construct(object* o, vector* args) {
    THROW_STATIC("base is not constructable", "<constructing>");
}

static void base_iconstruct(object* o, vector* args) {
    FAIL("base is not constructable");
}

ITER_SIMPLE(base)

typedef struct STRUCT_FUNC_DATA {
    char** commands;
    size_t len;
    obj_func caller;
} func_data;

typedef struct STRUCT_ITER {
    vector* values;
    size_t pos;
} iter;

static object* func_call(object* o, vector* args) {
    for (int i = 0; i < ((func_data*) o->value)->len; i++) {
        command_exec(((func_data*) o->value)->commands[i]);
        if (process_errors(false)) break;
    }
}

static void func_construct(object* o, vector* args) {
    func_data* fd = safe_malloc(sizeof(func_data));
    fd->caller = func_call;
    fd->commands = safe_calloc(VECTOR_LENGTH(args), sizeof(char*));
    fd->len = VECTOR_LENGTH(args);

    for (int i = 0; i < VECTOR_LENGTH(args); i++) {
        object* str = OBJECT_STR(((object*) vector_get(args, i)));
        if (process_errors(false)) return;
        fd->commands[i] = STRING_VALUE(str);
    }
}

static void func_dealloc(object* o) {
    if (((func_data*) o->value)->commands) free(((func_data*) o->value)->commands);
    map_free(o->attributes);
    free(o);
}

static object* func_to_string(object* o) {
    return string_from(STACK_DATA("[function]"));
}

ITER_SIMPLE(func)
ICONSTRUCT_SIMPLE(func, obj_func, "f")

static object* iterator_construct(object* o, vector* args) {}
static object* iterator_to_string(object* o) {}

static void iterator_iconstruct(object* o, vector* args) {
    iter* i = safe_malloc(sizeof(iter));
    i->pos = 0;
    i->values = vector_copy(args);
    o->value = i;
}

static void iterator_dealloc(object* o) {
    map_free(o->attributes);
    vector_free(((iter*) o)->values);
    free(o->value);
    free(o);
}

static object* iterator_next(object* o) {
    if (++((iter*) o)->pos >= VECTOR_LENGTH(((iter*) o)->values)) RETN(
        THROW_STATIC("reached end of iteration", "<iterating>");
    );
    return vector_get(((iter*) o)->values, ((iter*) o)->pos);
}

static object* iterator_iter(object* o) {
    return o;
}

ICONSTRUCT_SIMPLE(string, data*, "d")

static void string_construct(object* o, vector* args) {
    object* ob = vector_get(args, 0);
    if (!ob) {
        THROW_STATIC("not enough arguments", "<arguments>");
        return;
    }
    o->value = OBJECT_STR(o);
}

static object* string_to_string(object* o) {
    return o;
}

static void string_dealloc(object* o) {
    data_free(o->value);
    map_free(o->attributes);
    free(o);
}

static object* string_iter(object* o) {
    vector* v = vector_new();
    for (int i = 0; i < strlen(data_content(o->value)); i++) {
        char* str = safe_malloc(2);
        str[0] = ((char*) data_content(o->value))[i];
        str[1] = '\0';
        vector_append(v, HEAP_DATA(str));
    }

    return iterator_from(v);
}

static void array_construct(object* o, vector* args) {
    o->value = vector_copy(args);
}

static void array_iconstruct(object* o, vector* args) {
    o->value = vector_copy(args);
}

static void array_dealloc(object* o) {
    map_free(o->attributes);
    vector_free(o->value);
    free(o);
}

static object* array_to_string(object* o) {
    size_t size = 3;
    char* str = safe_malloc(size);
    size_t len = VECTOR_LENGTH(((vector*) o->value));
    str[0] = '[';

    for (int i = 0; i < len; i++) {
        object* current = vector_get(o->value, i);
        printf("%p\n", current);
        object* os = OBJECT_STR(current);

        if (error_occurred()) return NULL;

        char* s = STRING_VALUE(os);
        puts("hi");
        bool trailing_comma = (i + 1) != len;
        char* push_str = safe_malloc(strlen(s) + (trailing_comma ? 3 : 1));
        sprintf(push_str, "%s%s", s, trailing_comma ? ", " : "");
        str = safe_realloc(str, size + strlen(push_str));
        strcat(str, push_str);
    }

    str[size - 2] = ']';
    str[size - 1] = '\0';

    return string_from(HEAP_DATA(str));
}

static object* array_iter(object* o) {
    return iterator_from(o->value);
}

CONSTRUCT_SIMPLE(boolean, "b")
ITER_SIMPLE(boolean)

static object* boolean_to_string(object* o) {
    return string_from(STACK_DATA(o->value ? "true" : "false"));
}

static void boolean_dealloc(object* o) {
    map_free(o->attributes);
    free(o->value);
    free(o);
}

ICONSTRUCT_SIMPLE_CON(boolean, bool, "b")


ITER_SIMPLE(integer)

ICONSTRUCT_SIMPLE_CON(integer, int, "i")
CONSTRUCT_SIMPLE(integer, "i")

static void integer_dealloc(object* o) {
    map_free(o->attributes);
    free(o->value);
    free(o);
}

static object* integer_to_string(object* o) {
    char* str = safe_malloc(sizeof(char) * (int) log10(*((int*) o->value)));
    // ^^ i dont know what the hell the log10 thing does, i stole it from stack overflow
    sprintf(str, "%d", *((int*) o->value));
    return string_from(NOFREE_DATA(str));
}

/* Builtin types and objects */

type base;
type integer;
type func;
type string;
type boolean;
type array;
type iterator;

/* Scope functions */

/* Create a new scope */
scope* scope_new(void) {
    scope* s = safe_malloc(sizeof(scope));
    map* globals = map_new(1);
    
    BUILTIN(base);
    BUILTIN(integer);
    BUILTIN(func);
    BUILTIN(string);
    BUILTIN(boolean);
    BUILTIN(array);
    BUILTIN(iterator);

    s->global = globals;
    s->local = globals;

    return s;
}

/* Create a new scope using an existing global map. */
scope* scope_from(map* globals) {
    scope* s = safe_malloc(sizeof(scope));
    s->local = map_new(8);
    s->global = globals;
    return s;
}

static object* scope_map_get(scope* s, char* name) {
    void* value = map_get(s->local, name);
    if (!value) value = map_get(s->global, name);
    return value;
}

object* scope_get(scope* s, char* name) {
    char* token;
    object* last = NULL;

    while ((token = strsep(&name, "."))) {
        if (!last) {
            last = scope_map_get(s, token);
            if (!last) {
                char* str = safe_malloc(19 + strlen(token));
                sprintf(str, "unknown variable: %s", token);
                THROW_HEAP(str, "<lookup>");
                return NULL;
            }
        }
        else {
            last = map_get(last->attributes, token);
            if (!last) {
                char* str = safe_malloc(15 + strlen(token));
                sprintf(str, "no attribute: %s", token);
                THROW_HEAP(str, "<lookup>");
                return NULL;
            }
        }
    }

    return last;
}

static void scope_free_map(map* m) {
    map* types = map_new(m->capacity);
    data* b = NULL;

    for (int i = 0; i < m->capacity; i++) {
        pair* p = m->items[i];
        if (p) {
            if (type_compare(((object*) data_content(p->value))->tp, &base)) {
                if (!strcmp(data_content(p->key), "base")) {
                    b = data_from(p->value);
                } else map_set(types, data_from(p->key), data_from(p->value));
            }
        }
    }

    if (!b) FAIL("base object not found");

    map_free(m);
    map_free(types);
    
    data_free(b);
}

/* Free a scope (and its objects). */
void scope_free(scope* s, bool free_globals) {
    scope_free_map(s->local);
    if (free_globals) scope_free_map(s->global);
    free(s);
}


/* Type functions */

/* Initalize all types. */
void init_types(void) {
    LOADBUILTIN(base, "base");
    LOADBUILTIN(integer, "int");
    LOADBUILTIN(func, "function");
    func.call = func_call;
    LOADBUILTIN(boolean, "bool");
    LOADBUILTIN(string, "str");
    LOADBUILTIN(array, "array");
    LOADBUILTIN(iterator, "iterator");
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

inline object* object_dealloc(object* ob) {
    ob->tp->dealloc(ob);
}

static object* object_alloc(object* tp) {
    object* obj = safe_malloc(sizeof(object));
    obj->attributes = map_copy(tp->attributes);
    obj->tp = tp->value;
    obj->value = NULL;

    return obj;
}

static bool attributes_compare(map* a, map* b) {
    if (a == b) return true;
    if (a->capacity != b->capacity) return false;
    
    for (int i = 0; i < a->capacity; i++) {
        pair* p = a->items[i];
        pair* p2 = b->items[i];
        if (!(p && p2)) return false;
        if (strcmp(data_content(p->key), data_content(p2->key))) return false;
        if (!object_compare(data_content(p->value), data_content(p2->value))) return false;
    }

    return true;
}

bool type_compare(type* a, type* b) {
    if (a == b) return true; // if they are the exact same pointer, no need to check everything else
    if (!attributes_compare(a->attributes, b->attributes)) return false;
    if (a->call != b->call) return false;
    if (a->dealloc != b->dealloc) return false;
    if (a->to_string != b->to_string) return false;
    if (a->construct != b->construct) return false;
    if (a->iconstruct != b->iconstruct) return false;
    if (a->parent != b->parent) return false; // we can just compare the pointer here probably

    return true;
}

bool object_compare(object* a, object* b) {
    if (a == b) return true;
    if (!attributes_compare(a->attributes, b->attributes)) return false;
    if (a->tp != b->tp) return false;
    if (a->value != b->value) return false;
    return true;
}

/*
Instantiate a new object from a type.
 
Everything in params object should be a Control Manual object.
*/
object* object_new(object* tp, vector* params) {
    object* obj = object_alloc(tp);
    ((type*) tp->value)->construct(obj, params);

    return obj;
}

/* New object from a format. len should match the number of arguments. */
object* object_newf(object* tp, size_t len, ...) {
    VA_VEC(v, len);
    object* obj = object_new(tp, v);
    vector_free(v);

    return obj;
}

/* Create a new object using C value opposed to a Control Manual object. */
object* object_internal_new(object* tp, vector* params) {
    object* obj = object_alloc(tp);
    ((type*) tp->value)->iconstruct(obj, params);

    return obj;
}

/* Create a new internal object from a format. len should match the number of argument. */
object* object_internal_newf(object* tp, size_t len, ...) {
    VA_VEC(v, len);
    object* obj = object_internal_new(tp, v);
    vector_free(v);

    return obj;
}

/* Create an object from a type. NOT INSTANTIATING. */
object* object_from(data* tp) {
    object* o = safe_malloc(sizeof(object));
    o->attributes = map_copy(((type*) data_content(tp))->attributes);
    o->tp = &base;
    o->value = tp;
    return o;
}

/* Call the specified object. */
object* object_call(object* o, vector* args) {
    FAIL("not supported as of now");
}

/* Call an object with a format string. */
object* object_callf(object* o, size_t len, ...) {
    VA_VEC(v, len);
    object* obj = object_call(o, v);
    vector_free(v);
    return obj;
}

/* Utilities */

/* Integer from a C integer. */
object* integer_from(int value) {
    return object_internal_newf(GETOBJ(integer), 1, HEAP_DATA(int_convert(value)));
}

/* Function from a C function. */
object* func_from(obj_func function) {
    return object_internal_newf(GETOBJ(func), 1, STACK_DATA(function));
}

object* string_from(data* value) {
    return object_internal_newf(GETOBJ(string), 1, NOFREE_DATA(value));
}

object* iterator_from(vector* values) {
    return object_internal_newf(GETOBJ(iterator), 1, NOFREE_DATA(values));
}

object* array_from(vector* value) {
    return object_internal_newf(GETOBJ(array), 1, NOFREE_DATA(value));
}