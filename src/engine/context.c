#include <controlmanual/core/error.h> // THROW_STATIC
#include <controlmanual/core/map.h>
#include <controlmanual/core/object.h>
#include <controlmanual/core/util.h> // safe_malloc
#include <controlmanual/core/vector.h>
#include <controlmanual/engine/context.h>
#include <controlmanual/engine/lexer.h>
#include <controlmanual/engine/loader.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdio.h>  // sprintf
#include <string.h> // strlen
#define EXIT() longjmp(*c->buf, 1)

#define COMPARE(otp, ctp)                                                      \
  if (type_compare(p->tp, &otp)) {                                             \
    ctp* v = va_arg(args, ctp*);                                              \
    *v = o->value;                                                             \
    continue;                                                                  \
  }

#define COMPARE_DRF(otp, ctp)                                                  \
  if (type_compare(p->tp, &otp)) {                                             \
    ctp* v = va_arg(args, ctp*);                                              \
    *v = *((ctp*) o->value);                                                   \
    continue;                                                                  \
  }

context* context_new(
    command* co,
    vector* params,
    vector* flags,
    map* keywords,
    jmp_buf* buf
) {
    context* c = safe_malloc(sizeof(context));
    c->co = co;
    c->params = params;
    c->flags = flags;
    c->keywords = keywords;
    c->buf = buf;

    return c;
}

void context_free(context* c) {
    vector_free(c->params);
    vector_free(c->flags);
    map_free(c->keywords);
    free(c);
}

inline void context_abort(context* c) {
    EXIT();
}

void process_context_errors(context* c) {
    if (process_errors())
        context_abort(c);
}

void parse_context(context* c, ...) {
    va_list args;
    va_start(args, c);
    size_t current_pindex = 0;

    for (int i = 0; i < c->co->sc->params_len; i++) {
        param* p = c->co->sc->params[i];
        object* o = NULL;
        char* name = data_content(p->name);

        if (p->option) {
            option* ptr = va_arg(args, option*);
            o = vector_get(c->params, current_pindex);
            if (!o)
                o = map_get(c->keywords, name);
            else
                ++current_pindex;
            if (!o) {
                char* s = safe_malloc(26 + strlen(name));
                sprintf(s, "missing required argument: %s", name);
                THROW_HEAP(s, "<arguments>");
                EXIT();
            };

            if (!ensure_derives(o, &string))
                EXIT();
            int op_index = -1;

            for (int i = 0; i < p->options_size; i++) {
                if (!strcmp(p->options[i], STRING_VALUE(o))) {
                    op_index = i;
                    break;
                };
            }

            if (op_index == -1) {
                char* str = safe_malloc(28 + strlen(STRING_VALUE(o)));
                sprintf(str, "invalid option for choice: %s", STRING_VALUE(o));
                THROW_HEAP(str, "<arguments>");
                EXIT();
            };

            *ptr = op_index;
            continue;
        }

        if (p->flag) {
            bool* ptr = va_arg(args, bool*);
            bool found = false;

            for (int x = 0; x < VECTOR_LENGTH(c->flags); x++) {
                object* ob = VECTOR_GET(c->flags, x);
                if (!strcmp(STRING_VALUE(ob), name))
                    found = true;
                if (!strcmp(STRING_VALUE(ob), data_content(p->shorthand)))
                    found = true;
            }

            *ptr = found;
            continue;
        } else {
            o = vector_get(c->params, current_pindex);
            if (o)
                ++current_pindex;
            if (!o)
                o = map_get(c->keywords, name);
            if (!o) {
                char* shorthand = data_content_maybe(p->shorthand);
                if (shorthand)
                    o = map_get(c->keywords, shorthand);
            }
        }

        if (!o && p->required) {
            char* str = safe_malloc(28 + strlen(name));
            sprintf(str, "missing required argument: %s", name);
            THROW_HEAP(str, "<arguments>");
            EXIT();
        }

        if (!o && !p->required)
            // its up to the user to define a default value
            continue;

        if (p->tp == &any) {
            object** ob = va_arg(args, object * *);
            *ob = o;
            continue;
        };

        if (!ensure_derives(o, p->tp))
            EXIT();

        if (!p->convert) {
            object** ob = va_arg(args, object * *);
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
        EXIT();
    }

    va_end(args);
}

void missing(context* c, const char* name) {
    char* str = safe_malloc(28 + strlen(name));
    sprintf(str, "missing required argument: %s", name);
    THROW_HEAP(str, "<arguments>");
    EXIT();
}

void abort_ctx(context* c) {
    EXIT();
}

void throw_ctx(context* c, data* content, data* origin) {
    throw_error(content, origin, NULL, NULL);
    EXIT();
}
