#include <stdlib.h>
#include <utilities.h>
#include <cmobject.h>
#include <string.h>
#include <exception.h>

type_object* type_new(
    const char* name,
    map* attributes,
    const instance_init init,
    const instance_alloc alloc,
    const instance_dealloc dealloc
) {
    type_object* type = (type_object*) malloc(sizeof(type_object));
    if (!type) NOMEM("type_new");

    type->name = name;
    type->attributes = attributes;
    type->init = init;
    type->alloc = alloc;
    type->dealloc = dealloc;

    return type;
}

instance_object* instance_construct(
    type_object* type,
    vector* args
) {
    instance_object* obj = type->alloc(type);
    process_error();

    obj->attributes = map_new();
    memcpy(obj->attributes, type->attributes, sizeof(type->attributes));
    obj->type = type;
    obj->private = NULL;
    
    type->init(args, obj);
    process_error();

    vector_free(args);
    return obj;
}

void instance_free(instance_object* obj) {
    obj->type->dealloc(obj);
    process_error();

    map_free(obj->attributes);
    free(obj);
}

void type_free(type_object* type) {
    map_free(type->attributes);
    free(type);
}