#include <core/obj.h>

instance* type_instantiate(type* typ) {
    instance* i = malloc(sizeof(instance));
    i->base = typ;
}