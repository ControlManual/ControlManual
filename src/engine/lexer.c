#include <core/list.h>
#include <engine/lexer.h>
#include <core/data.h>
#include <core/error.h> // throw
#include <stdlib.h> // size_t
#include <string.h>
#include <core/util.h> // safe_malloc

#define PARSE_ERROR(content) { throw(STACK_DATA(content), STACK_DATA("<parsing>"), NULL); return NULL; }
#define FLAG(name, value) const unsigned int name = 0x##value

static token* token_new(token_type type, list* tokens, void* content) {
    token* t = safe_malloc(sizeof(token));
    t->content = content;
    t->type = type;
    t->tokens = tokens;
    return t;
}

FLAG(squote, 1);
FLAG(dquote, 2);
FLAG(array, 4);
FLAG(flag, 8);
FLAG(reference, 16);
FLAG(integer, 32);

list* tokenize(const char* str) {
    list* tokens = list_new();
    int flags = 0;
    size_t len = strlen(str);
    char* buf = safe_malloc(len + 1);

    for (int i = 0; i < len; i++) {
        char c = str[i];

        switch (c) {
            

            default:
                strcat(buf, &c);
                break; // do i need this?
        }
    }

    return tokens;
}