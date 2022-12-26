#include <engine/context.h>
#include <core/map.h>
#include <core/vector.h>
#include <engine/commands.h>
#include <core/util.h> // safe_malloc
#include <stdarg.h>
#include <core/object.h>
#include <core/error.h> // THROW_STATIC
#include <stdio.h> // sprintf
#include <engine/lexer.h>
#include <string.h> // strlen

#define COMPARE(otp, ctp) if (type_compare(p->tp, &otp)) { \
            ctp* v = va_arg(args, ctp*); \
            *v = o->value; \
            continue; \
        }

#define COMPARE_DRF(otp, ctp) if (type_compare(p->tp, &otp)) { \
            ctp* v = va_arg(args, ctp*); \
            *v = *((ctp*) o->value); \
            continue; \
        }

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
}

bool parse_context(context* c, ...) {
    va_list args;
    va_start(args, c);
    
    for (int i = 0; i < c->co->sc->params_len; i++) {
        param* p = c->co->sc->params[i];
        object* o = NULL;
        char* name = data_content(p->name);

        if (p->flag) {
            bool* ptr = va_arg(args, bool*);
            bool found = false;

            for (int x = 0; x < VECTOR_LENGTH(c->flags); x++) {
                object* ob = VECTOR_GET(c->flags, x);
                if (!strcmp(STRING_VALUE(ob), name)) found = true;
            }
            
            *ptr = found;
            continue;
        } else {
            o = vector_get(c->params, i);
            if (!o) o = map_get(c->keywords, name);
        }
        
        if (!o && p->required) { 
            char* str = safe_malloc(28 + strlen(name));
            sprintf(str, "missing required argument: %s", name);
            THROW_HEAP(
                str,
                "<arguments>"
            );
            return false;
        }
        
        if (!o && !p->required)
            // its up to the user to define a default value
            continue;

        if (p->tp == &any) {
            object** ob = va_arg(args, object**);
            *ob = o;
            continue;
        };

        if (!ensure_derives(o, p->tp)) return false;
        if (!p->convert) {
            object** ob = va_arg(args, object**);
            *ob = o;
            continue;
        }

        if (type_compare(p->tp, &string)) {
            char** v = va_arg(args, char**);
            *v = data_content(o->value);
            continue;
        }

        COMPARE_DRF(integer, int);
        COMPARE_DRF(boolean, bool);

        THROW_STATIC("unknown parameter type in schema", "<internal>");
        return false;
    }

    va_end(args);
    return true;
}