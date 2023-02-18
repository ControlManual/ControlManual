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
#define MEMBER(obj, name, flag) {#name, flag, offsetof(obj, name), 0, NULL}
#define METHOD(obj, name, flag) {#name, (PyCFunction) obj## _## name##, flag, NULL},
#define METHODV(obj, name) METHOD(obj, name, METH_VARARGS)
#define METHODN(obj, name) METHOD(obj, name, METH_NOARGS)
#define METHODC(obj, name) METHOD(obj, name, METH_CLASS)
#define PY_DATA(ob) data_new(Py_NewRef(ob), false, (data_dealloc) py_dealloc)
#define TYPE_HEAD(name, ...) static PyMethodDef name##_methods[] = { \
    __VA_ARGS__ \
    {NULL} \
}; \
static PyTypeObject name##Type = { \
    PyVarObject_HEAD_INIT(NULL, 0) \
    .tp_name = "controlmanual." #name, \
    .tp_basicsize = sizeof(name), \
    .tp_itemsize = 0, \
    .tp_flags = Py_TPFLAGS_DEFAULT, \
    .tp_new = PyType_GenericNew, \
    .tp_init = (initproc) name##_init, \
    .tp_new = (newfunc) name##_new, \
    .tp_methods = name##_methods, \
    .tp_dealloc = (destructor) name##_dealloc \
};

static void py_dealloc(PyObject* ob) {
    Py_DECREF(ob);
}

typedef struct {
    PyObject_HEAD;
    vector* v;
} Vector;

static int Vector_init(Vector* self, PyObject* args) {
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

static PyObject* Vector_new(PyTypeObject* tp, PyObject* args, PyObject* kwds) {
    Vector* self = (Vector*) tp->tp_alloc(tp, 0);
    if (!self) return NULL;
    self->v = vector_new();
    return (PyObject*) self;
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

TYPE_HEAD(Vector, METHODV(Vector, append))

static PyModuleDef module = {
    PyModuleDef_HEAD_INIT,
    .m_name = "controlmanual",
    .m_doc = "Python wrapper for Control Manual.",
    .m_size = -1,
};

PyMODINIT_FUNC PyInit_controlmanual(void)
{
    PyObject* m = PyModule_Create(&module);
    if (!m) return NULL;

    TP_READY(Vector);

    return m;
}