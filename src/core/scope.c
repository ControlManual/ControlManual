#include <core.h>
#define SFREE_ATTR(m, fn) for (int i = 0; i < m->size; i++) { \
        fn(m->items[i]->value); \
    };

scope* scope_new(scope* restrict glbl) {
    scope* s = malloc(sizeof(scope));
    if (!s) NOMEM("scope_new");
    s->glbl = glbl;
    s->objects = map_new();
    return s;
}

void scope_free(scope* restrict sc) {
    for (int i = 0; i < sc->objects->size; i++) {
        cm_object* current = sc->objects->items[i]->value;

        if (current->type->is_type) {
            type_free(current->type);
        } else {
            instance_free(current->instance);
        }
    }
    NONULL(sc, "scope_free");
}

void scope_add_type(scope* restrict sc, type_object* type) {
    map_add(sc->objects, type->name, type);
    NONULL(sc, "scope_free");
}