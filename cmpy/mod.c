#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <structmember.h>
#include <controlmanual/controlmanual.h>
#define TP_READY(obj) do { \
    if (PyType_Ready(&obj##Type) < 0) \
        return NULL; \
    Py_INCREF(&obj##Type); \
    if (PyModule_AddObject(m, #obj, (PyObject*) &obj##Type) < 0) { \
        Py_DECREF(&obj##Type); \
        Py_DECREF(m); \
        return NULL; \
    } \
} while (0)
#define CFUNC(obj, name) obj## _ ##name
#define MEMBER(obj, name, flag) {#name, flag, offsetof(obj, name), 0, NULL}
#define METHOD(obj, name, flag) {#name, (PyCFunction) CFUNC(obj, name), flag, NULL}
#define METHODV(obj, name) METHOD(obj, name, METH_VARARGS)
#define METHODN(obj, name) METHOD(obj, name, METH_NOARGS)
#define METHODC(obj, name) METHOD(obj, name, METH_CLASS)
#define PY_DATA(ob) data_new(Py_NewRef(ob), true, (data_dealloc) py_dealloc)
#define GETSET(name, target) {#target, (getter) name##_get_##target, (setter) name##_set_##target, NULL, NULL}
#define GET(name, target) {#target, (getter) name##_get_##target, NULL, NULL, NULL}
#define VARGS(...) { __VA_ARGS__, {NULL} }
#define VARGS_EMPTY() { {NULL} }
#define TYPE_HEAD(name, methods, members, getset) static PyMethodDef name##_methods[] = methods; \
static PyMemberDef name##_members[] = members; \
static PyGetSetDef name##_getset[] = getset; \
static PyTypeObject name##Type = { \
    PyVarObject_HEAD_INIT(NULL, 0) \
    .tp_name = "controlmanual." #name, \
    .tp_basicsize = sizeof(name), \
    .tp_itemsize = 0, \
    .tp_flags = Py_TPFLAGS_DEFAULT, \
    .tp_new = PyType_GenericNew, \
    .tp_init = (initproc) name##_init, \
    .tp_methods = name##_methods, \
    .tp_members = name##_members, \
    .tp_getset = name##_getset, \
    .tp_dealloc = (destructor) name##_dealloc \
};

#if PY_MAJOR_VERSION != 3
#error "only supported on python 3"
#endif

#if PY_MINOR_VERSION < 7
#error "python 3.7+ is required"
#endif

#if (PY_MINOR_VERSION < 10) || !defined(Py_NewRef)
static inline PyObject* py_newref(PyObject* o) {
    Py_INCREF(o);
    return o;
#ifdef Py_NewRef
#undef Py_NewRef
#endif
#define Py_NewRef py_newref
#endif

static void py_dealloc(PyObject* ob) {
    Py_DECREF(ob);
}

typedef struct {
    PyObject_HEAD;
    vector* v;
} Vector;

typedef struct {
    PyObject_HEAD;
    map* m;
} Map;

typedef struct {
    PyObject_HEAD;
    ui* u;
} UI;

typedef struct _Type Type;

struct _Type {
    type* tp;
    Type* parent;
    PyObject* name;
    PyObject* methods;
};

typedef struct {
    param* p;
    PyObject* name;
    PyObject* description;
    bool flag;
    bool keyword;
    bool required;
    PyObject* df;
    Type* tp;
    bool convert;
    PyObject* shorthand;
    bool option;
    PyObject* options;
} Param;

typedef struct {
    schema* sc;
    PyObject* name;
    PyObject* description;
    PyObject* params;
} Schema;

typedef struct {
    Schema* schema;
} Command;

typedef struct {
    PyObject_HEAD;
    context* c;
    Command* co;
    PyObject* params;
    PyObject* flags;
    PyObject* keywords;
} Context;

typedef struct {
    PyObject_HEAD;
    tcontext* tc;
    PyObject* name;
    PyObject* origin;
    PyObject* last;
    Context* ctx;
    int state;
} TContext;

typedef struct {
    PyObject_HEAD;
    error* e;
    PyObject* content;
    PyObject* expr;
    PyObject* origin;
    PyObject* problems;
    TContext* tc;
} Error;

static int validate_vector_index(Vector* self, Py_ssize_t index) {
    if (index < 0 || self->v->size <= index) {
        PyErr_Format(PyExc_IndexError, "bad index: %d\n", index);
        return -1;
    }

    return 0;
}

static int Vector_init(Vector* self, PyObject* args) {
    self->v = vector_new();
    PyObject* iterable = NULL;
    if (!PyArg_ParseTuple(args, "|O", &iterable)) return -1;
    if (!iterable) return 0;

    PyObject* iter = PyObject_GetIter(iterable);
    if (!iter) return -1;

    PyObject* item;
    while ((item = PyIter_Next(iter)))
        vector_append(self->v, PY_DATA(item));
    if (PyErr_Occurred()) return -1;

    return 0;
}

static void Vector_dealloc(Vector* self) {
    vector_free(self->v);
    Py_TYPE(self)->tp_free((PyObject*) self);
}

static PyObject* Vector_append(Vector* self, PyObject* args) {
    PyObject* ob;
    if (!PyArg_ParseTuple(args, "O", &ob)) return NULL;
    vector_append(self->v, PY_DATA(ob));
    Py_RETURN_NONE;
}

static PyObject* Vector_insert(Vector* self, PyObject* args) {
    Py_ssize_t index;
    PyObject* ob;
    if (!PyArg_ParseTuple(args, "nO", &index, &ob)) return NULL;
    vector_insert(self->v, index, PY_DATA(ob));
    Py_RETURN_NONE;
}

static PyObject* Vector_get(Vector* self, PyObject* args) {
    Py_ssize_t index;
    if (!PyArg_ParseTuple(args, "n", &index)) return NULL;
    PyObject* o = vector_get(self->v, index);
    if (!o) {
        PyErr_Format(PyExc_IndexError, "bad index: %zd", index);
        return NULL;
    }

    return Py_NewRef(o);
}

static PyObject* Vector_remove(Vector* self, PyObject* args) {
    Py_ssize_t index;
    if (!PyArg_ParseTuple(args, "n", &index)) return NULL;
    if (validate_vector_index(self, index) < 0) return NULL;
    vector_remove(self->v, index);
    Py_RETURN_NONE;
}

static PyObject* Vector_pop(Vector* self, PyObject* args) {
    Py_ssize_t index;
    if (!PyArg_ParseTuple(args, "n", &index)) return NULL;
    if (validate_vector_index(self, index) < 0) return NULL;
    return CONTENT_CAST(vector_pop(self->v, index), PyObject*);
}

static PyObject* Vector_get_length(Vector* self, void* closure) {
    return PyLong_FromSize_t(self->v->size);
}

static int Map_init(Map* self, PyObject* args) {
    Py_ssize_t initial;
    if (!PyArg_ParseTuple(args, "n", &initial)) return -1;
    self->m = map_new(initial);
    return 0;
}

static void Map_dealloc(Map* self) {
    map_free(self->m);
    Py_TYPE(self)->tp_free(self);
}

static PyObject* Map_set(Map* self, PyObject* args) {
    PyObject* ob;
    char* str; // this will be borrowed!
    if (!PyArg_ParseTuple(args, "sO", &str, &ob)) return NULL;
    map_set(self->m, HEAP_DATA(strdup(str)), PY_DATA(ob));
    Py_RETURN_NONE;
}

static PyObject* Map_get(Map* self, PyObject* args) {
    char* key;
    if (!PyArg_ParseTuple(args, "s", &key)) return NULL;
    PyObject* o = data_content_maybe(map_get_data(self->m, key));
    if (!o) {
        PyErr_Format(PyExc_KeyError, "invalid key: %s", key);
        return NULL;
    }

    return Py_NewRef(o);
}

static PyObject* Map_get_capacity(Map* self, void* closure) {
    return PyLong_FromSize_t(self->m->capacity);
}

static PyObject* Map_get_length(Map* self, void* closure) {
    return PyLong_FromSize_t(self->m->len);
}

static int UI_init(UI* self, PyObject* args) {
    if (!cm_impl_ui_wrapper) {
        PyErr_SetString(PyExc_RuntimeError, "no UI is available");
        return -1;
    }

    self->u = cm_impl_ui_wrapper;
    return 0;
}

static void UI_dealloc(UI* self) {
    Py_TYPE(self)->tp_free(self);
}

static PyObject* UI_error(UI* self) {
    if (!self->u->error) {
        PyErr_SetString(PyExc_TypeError, "error is not set on the ui");
        return NULL;
    }
    Py_RETURN_NONE;
}

static int TContext_from_ptr(TContext* ob, tcontext* tc) {
    if (tc->last) {
        TContext* tc_ob = (TContext*) Py_TYPE(ob)->tp_alloc(Py_TYPE(ob), 0);
        if (TContext_from_ptr(tc_ob, tc->last) < 0) return -1;
        ob->last = (PyObject*) tc_ob;
    }
    ob->name = PyUnicode_FromString(CONTENT_STR(tc->name));
    if (!ob->name) return -1;
    ob->origin = PyUnicode_FromString(CONTENT_STR(tc->name));
    if (!ob->origin) return -1;
    ob->state = tc->state;

    return 0;
}

static int TContext_init(TContext* self, PyObject* args) {
    if (!cm_runtime_tcontext) {
        PyErr_SetString(PyExc_RuntimeError, "tcontext runtime is not initalized");
        return -1;
    }
    
    tcontext* tc = TC();
    if (TContext_from_ptr(self, tc) < 0) return -1;
    return 0;
}

static void TContext_dealloc(TContext* self) {
    Py_XDECREF(self->name);
    Py_XDECREF(self->origin);
    Py_XDECREF(self->last);
    Py_XDECREF(self->ctx);
    Py_TYPE(self)->tp_free(self);
}
static PyTypeObject TContextType;
static PyTypeObject* ContextType;

static PyObject* TContext_advance(TContext* self, PyObject* args) {
    char* name;
    char* origin;
    tcontext_state state;
    Context* ctx = NULL;

    if (!PyArg_ParseTuple(args, "ssiO!", &name, &origin, &state, &ContextType, &ctx)) return NULL;
    tcontext_advance(HEAP_DATA(strdup(name)), HEAP_DATA(strdup(origin)), ctx ? ctx->c : NULL, state);
    Py_RETURN_NONE;
}

static PyObject* TContext_pop() {
    tcontext_pop();
    Py_RETURN_NONE;
}

static int Type_init(Type* self, PyObject* args) {
    PyErr_SetString(PyExc_TypeError, "types cannot be constructed from python");
    return -1;
}

static void Type_dealloc(Type* self) {
    Py_XDECREF(self->name);
    Py_XDECREF(self->methods);
    Py_XDECREF(self->parent);
    Py_TYPE(self)->tp_free(self);
}

static int Type_from_ptr(Type* ob, type* tp) {
    if (tp->parent) {
        Type* ob_tp = (Type*) Py_TYPE(ob)->tp_alloc(Py_TYPE(ob), 0);
        if (Type_from_ptr(ob_tp, tp->parent) < 0) return -1;
    }
    ob->name = PyUnicode_FromString(CONTENT_STR(tp->name));
    if (!ob->name) return -1;
    ob->methods = PyDict_New();
    if (!ob->methods) return -1;

    return 0;
}

static void Error_dealloc(Error* self) {
    Py_XDECREF(self->content);
    Py_XDECREF(self->tc);
    Py_XDECREF(self->expr);
    Py_XDECREF(self->origin);
    Py_XDECREF(self->origin);
    Py_TYPE(self)->tp_free(self);
}

static int Error_from_ptr(Error* ob, error* er) {
    ob->content = PyUnicode_FromString(CONTENT_STR(er->content));
    if (!ob->content) return -1;
    ob->expr = PyUnicode_FromString(CONTENT_STR(er->expr));
    if (!ob->expr) return -1;
    TContext* tc = (TContext*) TContextType.tp_alloc(&TContextType, 0);
    if (!tc) return -1;
    if (TContext_from_ptr(tc, er->tc) < 0) return -1;

    ob->tc = tc;
    ob->origin = PyUnicode_FromString(CONTENT_STR(er->origin));
    if (!ob->origin) return -1;
    ob->e = er;
    
    if (er->problems) {
        PyObject* tuple = PyTuple_New(VECTOR_LENGTH(er->problems));
        if (!tuple) return -1;
            
        for (int i = 0; i < VECTOR_LENGTH(er->problems); ++i) {
            PyTuple_SET_ITEM(tuple, i, PyLong_FromLong(*((int*) VECTOR_GET(er->problems, i))));
        }

        ob->problems = tuple;
    }

    return 0;
}

static int Error_init(Error* self, PyObject* args) {
    return 0;
}


TYPE_HEAD(
    Vector,
    VARGS(
        METHODV(Vector, append),
        METHODV(Vector, insert),
        METHODV(Vector, get),
        METHODV(Vector, remove),
        METHODV(Vector, pop)
    ),
    VARGS_EMPTY(),
    VARGS(GET(Vector, length))
);

TYPE_HEAD(
    Map,
    VARGS(
        METHODV(Map, set),
        METHODV(Map, get)
    ),
    VARGS_EMPTY(),
    VARGS(GET(Map, capacity), GET(Map, length))
);

TYPE_HEAD(
    TContext,
    VARGS(
        METHODC(TContext, advance),
        METHODN(TContext, pop)
    ),
    VARGS(
        MEMBER(TContext, state, T_INT),
        MEMBER(TContext, last, T_OBJECT),
        MEMBER(TContext, name, T_OBJECT_EX),
        MEMBER(TContext, origin, T_OBJECT_EX),
        MEMBER(TContext, ctx, T_OBJECT)
    ),
    VARGS_EMPTY()
);

TYPE_HEAD(
    Type,
    VARGS_EMPTY(),
    VARGS(
        MEMBER(Type, parent, T_OBJECT),
        MEMBER(Type, name, T_OBJECT_EX),
        MEMBER(Type, methods, T_OBJECT_EX)
    ),
    VARGS_EMPTY()
)

TYPE_HEAD(
    Error,
    VARGS_EMPTY(),
    VARGS(
        MEMBER(Error, content, T_OBJECT_EX),
        MEMBER(Error, origin, T_OBJECT_EX),
        MEMBER(Error, expr, T_OBJECT),
        MEMBER(Error, problems, T_OBJECT),
        MEMBER(Error, tc, T_OBJECT_EX)
    ),
    VARGS_EMPTY()
)

int converter(PyObject* ob, void** target) {
    if (!PyCallable_Check(ob)) {
        *target = ob;
        return 1;
    }

    return 0;
}

PyObject* wrapper_error = NULL;
PyObject* wrapper_warn = NULL;
PyObject* wrapper_print = NULL;
PyObject* wrapper_input = NULL;
PyObject* wrapper_start = NULL;
PyObject* wrapper_end = NULL;
PyObject* wrapper_help = NULL;
PyObject* wrapper_alert = NULL;
PyObject* wrapper_confirm = NULL;
PyObject* wrapper_window = NULL;
PyObject* wrapper_window_close = NULL;
PyObject* wrapper_window_write = NULL;
PyObject* wrapper_clear = NULL;
PyObject* wrapper_choose = NULL;
PyObject* wrapper_list = NULL;
PyObject* wrapper_log = NULL;

#define CV(ob) &converter, wrapper_ ##ob

void ui_impl_error(error* e, tcontext* tc) {
    if (!wrapper_error) Py_FatalError("error wrapper has not been set");
    TContext* tctx = (TContext*) TContextType.tp_alloc(&TContextType, 0);
    if (!tctx) PyErr_Print();
    TContext_from_ptr(tctx, tc);
    if (!PyObject_CallFunction(wrapper_error, "OO")) {
        PyErr_Print();
    }
}

static PyObject* register_ui(PyObject* cls, PyObject* args) {
    if (!PyArg_ParseTuple(
        args,
        "O&O&O&O&O&O&O&O&O&O&O&O&O&O&O&O&",
        CV(error),
        CV(warn),
        CV(print),
        CV(input),
        CV(start),
        CV(end),
        CV(help),
        CV(alert),
        CV(confirm),
        CV(window),
        CV(window_close),
        CV(window_write),
        CV(clear),
        CV(choose),
        CV(list),
        CV(log)
    )) return NULL;

    Py_RETURN_NONE;
}

TYPE_HEAD(
    UI,
    VARGS(
        METHODC(UI, register_ui)
    )
    VARGS_EMPTY(),
    VARGS_EMPTY()
)

static PyModuleDef module = {
    PyModuleDef_HEAD_INIT,
    .m_name = "controlmanual",
    .m_doc = "Python wrapper for Control Manual.",
    .m_size = -1,
};

PyMODINIT_FUNC PyInit_controlmanual(void) {
    PyObject* m = PyModule_Create(&module);
    if (!m) return NULL;
    TP_READY(Vector);
    TP_READY(Map);
    TP_READY(TContext);
    TP_READY(Type);
    TP_READY(UI);

    return m;
}
