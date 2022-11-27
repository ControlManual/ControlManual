#include <engine/context.h>
#include <core/map.h>
#include <core/vector.h>
#include <engine/commands.h>
#include <core/util.h> // safe_malloc
#include <stdarg.h>

context* context_new(command* co, vector* params, vector* flags, map* keywords) {
    context* c = safe_malloc(sizeof(context));
    c->co = co;
    c->params = params;
    c->flags = flags;
    c->keywords = keywords;

    return c;
}

void context_free(context* c) {
    vector_free(c->params);
    vector_free(c->flags);
    map_free(c->keywords);
    free(c);
    return c;
}

bool parse_context(context* c, ...) {
    va_list args;
    va_start(args, c);
    
    for (int i = 0; i < c->co->sc->params_len; i++) {
        param* p = c->co->sc->params[i];
        
    }

    va_end(args);
    return true;
}