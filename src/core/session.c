#include <cm.h>

scope* scope_new(session* restrict ses) {
    scope* s = (scope*) malloc(sizeof(scope));
    if (!s) NOMEM("scope_new");
    s->parent = ses;
    s->instances = map_new();
    s->types = map_new();
    return s;
}

session* session_new(void) {
    if (!string) FATAL("session_new", "builtin types have not been set up");
    session* s = (session*) malloc(sizeof(session));
    if (!s) NOMEM("session_new");
    
    s->global = scope_new(s);
    s->scopes = vector_new();

    return s;
}

void session_add_type(scope* restrict sc, type_object* restrict type) {
    map_add(sc->types, (char*) type->name, type);
}

