#ifndef __CM_OBJECT_H__
#define __CM_OBJECT_H__
#include <utilities.h>
#include <stdbool.h>

typedef struct STRUCT_OBJECT instance_object;
typedef struct STRUCT_TYPE type_object;

typedef void (*instance_init)(instance_object*, vector*);
typedef instance_object* (*instance_alloc)(type_object*);
typedef void (*instance_imethod)(instance_object*);
typedef char* (*instance_sgmethod)(instance_object*);

struct STRUCT_TYPE {
    const char* name;
    map* attributes;
    instance_init init;
    instance_alloc alloc;
    instance_imethod dealloc;
    instance_sgmethod to_string;
};

struct STRUCT_OBJECT {
    type_object* type;
    map* attributes;
    void* private;
};

type_object* type_new(
    const char* name,
    map* attributes,
    const instance_init init,
    const instance_alloc alloc,
    const instance_imethod dealloc,
    const instance_sgmethod to_string
);

instance_object* instance_construct(
    type_object* type,
    vector* args
);

void instance_free(instance_object* obj);
void type_free(type_object* type);

extern char* instance_to_string(instance_object* obj);

#endif