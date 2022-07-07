#ifndef __CM_OBJECT_H__
#define __CM_OBJECT_H__
#include <utilities.h>
#include <stdbool.h>

/* Type representing an object structure. */
typedef struct STRUCT_OBJECT instance_object;
/* Type representing an type object structure. */
typedef struct STRUCT_TYPE type_object;

/* Instance initalization method. Called after the instance object has been constructed and allocated from its parent type. */
typedef void (*instance_init)(instance_object*, vector*);
/* Instance allocation method. Used to allocate a instance object from a type. */
typedef instance_object* (*instance_alloc)(type_object*);
/* Instance internal method. Handles internal data for the target instance object. */
typedef void (*instance_imethod)(instance_object*);
/* Instance string-get method. Gets a string from the target instance object. */
typedef char* (*instance_sgmethod)(instance_object*);

/* Struct holding data for type objects. */
struct STRUCT_TYPE {
    const char* name;
    map* attributes;
    instance_init init;
    instance_alloc alloc;
    instance_imethod dealloc;
    instance_sgmethod to_string;
    const type_object* extends;
};

/* Struct holding data for instance objects. */
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
    const instance_sgmethod to_string,
    const type_object* extends
);

instance_object* instance_construct(
    type_object* type,
    vector* args
);

void instance_free(instance_object* obj);
void type_free(type_object* type);

extern char* instance_to_string(instance_object* obj);

#endif