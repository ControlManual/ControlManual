#ifndef CM_OBJECT_H
#define CM_OBJECT_H

#include <stdbool.h>
#include "scope.h"
#include "map.h"

#define OBJ_BASE
    scope* object_scope; \
    map* attributes;

typedef struct STRUCT_OBJECT {
    OBJ_BASE;
} object;

typedef struct _instance instance;
typedef struct _type type;

typedef void (*type_construct)(instance*);
typedef instance* (*type_alloc)(type*);
typedef void (*type_dealloc)(instance*);

struct STRUCT_TYPE {
    OBJ_BASE;
    type_construct constructor;
    type_alloc alloc;
    type_dealloc dealloc;
    char* name;
};

struct STRUCT_INSTANCE {
    OBJ_BASE;
    type* base;
};

#endif
