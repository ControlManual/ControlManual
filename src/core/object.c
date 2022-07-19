#include <stdlib.h>
#include <utilities.h>
#include <cmobject.h>
#include <string.h>
#include <exception.h>
#define EXTATTR(name) if (!name) type->name = extends->name;

/* Generate a new type object. */
type_object* type_new(
    const char* name,
    map* restrict attributes,
    const instance_init init,
    const instance_alloc alloc,
    const instance_imethod dealloc,
    const instance_sgmethod to_string,
    const type_object* restrict extends
) {
    type_object* type = malloc(sizeof(type_object));
    if (!type) NOMEM("type_new");

    type->name = name;
    type->attributes = attributes;
    type->init = init;
    type->alloc = alloc;
    type->dealloc = dealloc;
    type->to_string = to_string;
    type->extends = extends;
    type->is_type = true;

    if (extends) {
        EXTATTR(init);
        EXTATTR(alloc);
        EXTATTR(dealloc);
        EXTATTR(to_string);
        for (int i = 0; i < extends->attributes->size; i++) {
            char* key = extends->attributes->items[i]->key;
            map_get(attributes, key);
            CATCH_ERROR;

            if (err) {
                map_add(attributes, key, map_get(extends->attributes, key));
                free(err);
            }

            process_error();
        }
    }

    return type;
}

/* Generate a new instance object from a type. */
instance_object* instance_construct(
    type_object* restrict type,
    vector* restrict args
) {
    instance_object* obj = type->alloc(type);
    process_error();
    obj->is_type = false;

    obj->attributes = map_new();
    memmove(obj->attributes, type->attributes, sizeof(type->attributes));
    obj->attributes->items = (_kv**) calloc(0, 0);

    obj->type = type;
    obj->private = NULL;
    
    type->init(obj, args);
    process_error();

    vector_free(args);
    return obj;
}

/* Free an instance of a type. */
void instance_free(instance_object* restrict obj) {
    NONULL(obj, "instance_free");
    if (obj->type->dealloc) {
        obj->type->dealloc(obj);
        process_error();
    }
    map_free(obj->attributes);
    free(obj);
}

/* Free a type object. */
void type_free(type_object* restrict type) {
    NONULL(type, "type_free");
    map_free(type->attributes);
    free(type);
}

/* Convert an instance object to a string. */
inline char* instance_to_string(instance_object* restrict obj) {
    return (char*) obj->type->to_string(obj);
}
