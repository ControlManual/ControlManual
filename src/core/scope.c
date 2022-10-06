#include <core/scope.h>
#include <core/error.h>
#include <stdlib.h>

scope* scope_new(map* globals) {
    scope* s = safe_malloc(sizeof(scope));
    s->globals = globals ? globals : map_new();
    s->locals = map_new();

    return s;
}

scope* scope_free(scope* s) {
    if (s->globals) map_free(s->globals);
    map_free(s->locals);
    free(s);
}