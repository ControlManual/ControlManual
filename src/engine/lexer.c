#include <core/vector.h>
#include <engine/lexer.h>
#include <core/data.h>
#include <core/error.h> // throw
#include <stdlib.h> // size_t
#include <string.h>
#include <core/util.h> // safe_malloc, RETN
#include <stdio.h> // sprintf
#include <engine/util.h> // char_to_string
#include <core/object.h>
#include <core/error.h> // THROW_STATIC

#define PARSE_ERROR(content) RETN(parse_error(len, str, content, i))
#define DUMPBUF() if (strlen(buf)) { \
            vector_append(tokens, CUSTOM_DATA(btoken_new(NOTOK, strdup(buf)), btoken_dealloc)); \
            buf = safe_malloc(len + 1); \
            strcpy(buf, ""); \
}
        
#define DUMP(tok) DUMPBUF(); \
        vector_append(tokens, HEAP_DATA(btoken_new(tok, NULL))); \
        vector_remove(stack, VECTOR_LENGTH(stack) - 1); \

#define PUSH(flags, tok) vector_append(stack, HEAP_DATA(stackitem_new(flags, i))); \
    DUMPBUF(); \
    vector_append(tokens, HEAP_DATA(btoken_new(tok, NULL)));
#define PUSH_AND_DUMP(tok) DUMPBUF(); \
    vector_append(tokens, CUSTOM_DATA(btoken_new(tok, char_to_string(c)), btoken_dealloc));
#define CHECK(flags) (current_flags & (flags)) == (flags)
#define WRITE() strncat(buf, char_to_string(c), len);
#define STACK_PUSH(item) vector_append( \
                    vector_get(token_stack, VECTOR_LENGTH(token_stack) - 1), \
                    item \
                )
#define WRITE_BUFFER() strcat(buf, btok->content)
#define CLEAR_BUFFER_NOWRITE() free(buf); buf = safe_malloc(len + 1); strcpy(buf, "");
#define CLEAR_BUFFER() if (strlen(buf)) { \
        STACK_PUSH(token_new(is_digit ? INTEGER_LITERAL : STRING_LITERAL, strdup(buf))); \
        is_digit = false; \
    } \
    CLEAR_BUFFER_NOWRITE();

typedef enum ENUM_BTOKEN_TYPE {
    NOTOK,
    DSTRING_OPEN,
    DSTRING_CLOSE,
    SSTRING_OPEN,
    SSTRING_CLOSE,
    ARRAY_OPEN,
    COMMA,
    ARRAY_CLOSE,
    DIGIT,
    PAREN_OPEN,
    PAREN_CLOSE,
    WHITESPACE,
    BRACKET_OPEN,
    BRACKET_CLOSE,
} btoken_type;

typedef struct STRUCT_BTOKEN {
    btoken_type type;
    char* content;
} btoken;

static btoken* btoken_new(btoken_type type, void* content) {
    btoken* t = safe_malloc(sizeof(btoken));
    t->content = content;
    t->type = type;
    return t;
}

typedef struct STRUCT_STACKITEM {
    unsigned int flags;
    size_t index;
} stackitem;

static stackitem* stackitem_new(unsigned int flags, size_t index) {
    stackitem* s = safe_malloc(sizeof(stackitem));
    s->flags = flags;
    s->index = index;
    return s;
}

static callexpr* callexpr_new(vector* tokens, token* name) {
    callexpr* c = safe_malloc(sizeof(callexpr));
    c->tokens = tokens;
    c->name = name;
    return c;
}

typedef enum ENUM_BASICFLAG {
    ignore_toks = 1 << 0,
    disallow_whitespace = 1 << 1,
    is_squote = 1 << 2,
    is_dquote = 1 << 3,
    is_array = 1 << 4,
    is_paren = 1 << 5,
    is_bracket = 1 << 6,
} basicflag;

static inline void btoken_dealloc(btoken* tok) {
    free(tok->content);
    free(tok);
}

static void parse_error(size_t len, const char* str, char* er_str, size_t i) {
    vector* v = vector_new();
    vector_append(v, HEAP_DATA(int_convert(i)));

    throw(
        STACK_DATA(er_str),
        STACK_DATA("<parsing>"),
        NULL,
        NOFREE_DATA((char*) str),
        v
    );
}

vector* tokenize_basic(const char* str) {
    vector* tokens = vector_new();
    size_t len = strlen(str);
    char* buf = safe_malloc(len + 1);
    strcpy(buf, "");
    vector* stack = vector_new();
    unsigned int current_flags;

    for (int i = 0; i < len; i++) {
        char c = str[i];
        
        stackitem* item = vector_get(stack, VECTOR_LENGTH(stack) - 1);
        current_flags = item ? item->flags : 0;

        switch (c) {
            case '"': {
                if (CHECK(is_dquote)) {
                    DUMP(DSTRING_CLOSE);
                    break;
                }
                
                PUSH(ignore_toks | is_dquote, DSTRING_OPEN);
                break;
            }

            case '\'': {
                if (CHECK(is_squote)) {
                    DUMP(SSTRING_CLOSE);
                    break;
                }
                PUSH(ignore_toks | is_squote, SSTRING_OPEN);
                break;
            }

            case ' ': {
                if (CHECK(disallow_whitespace)) {
                    PARSE_ERROR("whitespace is not allowed here");
                }

                if (CHECK(ignore_toks) == 0) {
                    PUSH_AND_DUMP(WHITESPACE);
                } else WRITE();
                break;
            }
            
            case '[': {
                if (CHECK(ignore_toks) == 0) {
                    PUSH(is_array, ARRAY_OPEN);
                    break;
                }
                WRITE();
                break;
            }

            case ']': {
                if (CHECK(ignore_toks) == 0) {
                    if (CHECK(is_array)) {
                        DUMP(ARRAY_CLOSE);
                        break;
                    }
                    PARSE_ERROR("unexpected ']'")
                }
                WRITE();
                break;
            }

            case ',': {
                if (CHECK(ignore_toks) == 0) {
                    if (CHECK(is_array) || CHECK(is_paren)) {
                        PUSH_AND_DUMP(COMMA);
                        break;
                    }

                    PARSE_ERROR("unexpected ','");
                }
                WRITE();
                break;
            }

            case '(': {
                if (CHECK(ignore_toks) == 0) {
                    PUSH(is_paren, PAREN_OPEN);
                    break;
                }
                WRITE();
                break;
            }

            case ')': {
                if (CHECK(ignore_toks) == 0) {
                    if (CHECK(is_paren)) {
                        DUMP(PAREN_CLOSE);
                        break;
                    }
                    PARSE_ERROR("unexpected ')'");
                }
                WRITE();
                break;
            }

            case '{': {
                if (CHECK(ignore_toks) == 0) {
                    PUSH(is_bracket | disallow_whitespace, BRACKET_OPEN);
                    break;
                }
                WRITE();
                break;
            }

            case '}': {
                if (CHECK(ignore_toks) == 0) {
                    if (CHECK(is_bracket)) {
                        DUMP(BRACKET_CLOSE);
                        break;
                    }
                    PARSE_ERROR("unexpected '}'");
                }
                WRITE();
                break;
            }


            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            {
                if (CHECK(ignore_toks) == 0) {
                    PUSH_AND_DUMP(DIGIT);
                } else WRITE();
                break;
            }

            default:
                WRITE();
        }
    }
    
    if (strlen(buf))
        vector_append(
            tokens,
            CUSTOM_DATA(
                btoken_new(NOTOK, strdup(buf)),
                btoken_dealloc
            )
        );
   

    if (VECTOR_LENGTH(stack) != 0) RETN(
            parse_error(
                len,
                str,
                "unterminated token",
                ((stackitem*) vector_get(
                    stack,
                    VECTOR_LENGTH(stack) - 1
                ))->index
            )
        );
    vector_free(stack);

    return tokens;
}

static data* token_new(token_type type, void* content) {
    token* t = safe_malloc(sizeof(btoken));
    t->content = content;
    t->type = type;
    return HEAP_DATA(t);
}

static inline void token_dealloc(token* tok) {
    free(tok->content);
    free(tok);
}

vector* tokenize(const char* str) {
    vector* basic = tokenize_basic(str);
    if (!basic) return NULL;
    vector* tokens = vector_new();
    vector* token_stack = vector_new();
    vector_append(token_stack, HEAP_DATA(tokens));
    size_t len = strlen(str);
    char* buf = safe_malloc(len + 1);
    strcpy(buf, "");
    bool is_digit = false;

    for (int i = 0; i < VECTOR_LENGTH(basic); i++) {
        btoken* btok = vector_get(basic, i);

        switch (btok->type) {
            case DSTRING_CLOSE:
            case SSTRING_CLOSE:
                STACK_PUSH(token_new(STRING_LITERAL, strdup(buf)));
                CLEAR_BUFFER();
                break;

            case BRACKET_CLOSE:
                STACK_PUSH(token_new(REFERENCE, strdup(buf)));
                CLEAR_BUFFER_NOWRITE();
                break;
                
            case WHITESPACE:
            case COMMA:
            case DSTRING_OPEN:
            case SSTRING_OPEN:
                CLEAR_BUFFER();
                break;
           
            case DIGIT:
                if (!strlen(buf) && !is_digit) is_digit = true;
                WRITE_BUFFER();
                break;

            case PAREN_OPEN:
            case ARRAY_OPEN:
                CLEAR_BUFFER();
                vector_append(token_stack, HEAP_DATA(vector_new()));
                break;
            
            case PAREN_CLOSE:
                CLEAR_BUFFER();
                vector* group = (vector*) data_content(
                    vector_pop(
                        token_stack, VECTOR_LENGTH(token_stack) - 1
                    )
                );
                vector* tstack = vector_get(
                    token_stack,
                    VECTOR_LENGTH(token_stack) - 1
                );
                token_type tp = ((token*)
                    vector_get(
                        tstack,
                        VECTOR_LENGTH(tstack) - 1
                    )
                )->type == STRING_LITERAL ? CALL : GROUP_LITERAL;
                
                void* content;

                if (tp == CALL) {
                    content = callexpr_new(
                        group,
                        data_content(
                            vector_pop(tstack, VECTOR_LENGTH(tstack) - 1)
                        )
                    );
                }  else {
                    content = group;
                }

                STACK_PUSH(
                    token_new(
                        tp,
                        content
                    )
                );
                break;

            case ARRAY_CLOSE:
                CLEAR_BUFFER();
                data* array = vector_pop(
                    token_stack,
                    VECTOR_LENGTH(token_stack) - 1
                );
                STACK_PUSH(token_new(ARRAY_LITERAL, data_content(array)));
                break;

            case NOTOK:
                is_digit = false;
                WRITE_BUFFER();
                break;
        }
    }

    CLEAR_BUFFER();

    vector_free(basic);
    //vector_free(token_stack);
    free(buf);

    return tokens;
}


vector* params_from_tokens(vector* tokens, char** command_name) {
    vector* params = vector_new();
    if (VECTOR_LENGTH(tokens) == 0) return NULL;
    data* name = vector_pop(tokens, 0);
    token* t = data_content(name);
    if (t->type != STRING_LITERAL) RETN(
        THROW_STATIC("command name must be a string", "<parsing>")
    )
    *command_name = t->content;

    data_free(name);

    for (int i = 0; i < VECTOR_LENGTH(tokens); i++) {
        token* t = vector_get(tokens, i);
        switch (t->type) {
            case STRING_LITERAL:
                vector_append(
                    params,
                    HEAP_DATA(string_from(NOFREE_DATA(t->content)))
                );
                break;
            case INTEGER_LITERAL:
                // TODO: make sure integer limit isnt hit here
                int value;
                sscanf(t->content, "%d", &value);

                vector_append(params, HEAP_DATA(integer_from(value)));
                break;
        }
    }

    return params;
}