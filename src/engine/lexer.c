#include <core/vector.h>
#include <engine/lexer.h>
#include <core/data.h>
#include <core/error.h> // throw
#include <stdlib.h> // size_t
#include <string.h>
#include <core/util.h> // safe_malloc, RETN
#include <stdio.h> // sprintf

#define PARSE_ERROR(content) RETN(parse_error(len, str, content, i))
#define DUMPBUF() if (strlen(buf)) { \
            vector_append(tokens, CUSTOM_DATA(btoken_new(NOTOK, strdup(buf), i), btoken_dealloc)); \
            buf = safe_malloc(len + 1); \
            strcpy(buf, ""); \
}
        
#define DUMP(tok) DUMPBUF(); \
        vector_append(tokens, HEAP_DATA(btoken_new(tok, NULL, i))); \
        vector_remove(stack, VECTOR_LENGTH(stack) - 1); \

#define PUSH(flags, tok) vector_append(stack, HEAP_DATA(int_convert(flags))); \
    DUMPBUF(); \
    vector_append(tokens, HEAP_DATA(btoken_new(tok, NULL, i)));
#define PUSH_AND_DUMP(tok) DUMPBUF(); \
    vector_append(tokens, CUSTOM_DATA(btoken_new(tok, char_to_string(c), i), btoken_dealloc));
#define CHECK(flags) (current_flags & (flags)) == (flags)
#define WRITE() strncat(buf, char_to_string(c), len);
#define STACK_PUSH(item) vector_append( \
                    vector_get(token_stack, VECTOR_LENGTH(token_stack) - 1), \
                    item \
                )
#define WRITE_BUFFER() strcat(buf, btok->content)
#define CLEAR_BUFFER() free(buf); buf = safe_malloc(len + 1); strcpy(buf, "");

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
    size_t index;
} btoken;

static btoken* btoken_new(btoken_type type, void* content, size_t index) {
    btoken* t = safe_malloc(sizeof(btoken));
    t->content = content;
    t->type = type;
    t->index = index;
    return t;
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

static unsigned int* int_convert(unsigned int value) {
    unsigned int* i = safe_malloc(sizeof(unsigned int));
    *i = value;
    return i;
}

static char* char_to_string(char c) {
    char* str = safe_malloc(2);
    str[0] = c;
    str[1] = '\0';
    return str;
}

static void parse_error(size_t len, const char* str, char* er_str, size_t i) {
    vector* v = vector_new();
    vector_append(v, HEAP_DATA(int_convert(i)));

    throw(STACK_DATA(er_str), STACK_DATA("<parsing>"), NULL, NOFREE_DATA((char*) str), v);
}

vector* tokenize_basic(const char* str) {
    vector* tokens = vector_new();
    size_t len = strlen(str);
    char* buf = safe_malloc(len + 1);
    strcpy(buf, "");
    vector* stack = vector_new();
    int current_flags;

    for (int i = 0; i < len; i++) {
        char c = str[i];
        
        void* item = vector_get(stack, VECTOR_LENGTH(stack) - 1);
        current_flags = item ? *((unsigned int*) item) : 0;
        
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
                    PUSH(is_bracket, BRACKET_OPEN);
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
        vector_append(tokens, CUSTOM_DATA(btoken_new(NOTOK, strdup(buf), len - 1), btoken_dealloc));
    
    if (VECTOR_LENGTH(stack) != 0) RETN(parse_error(len, str, "unterminated token", ((btoken*) vector_get(stack, VECTOR_LENGTH(stack) - 1))->index));
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

    for (int i = 0; i < VECTOR_LENGTH(basic); i++) {
        btoken* btok = vector_get(basic, i);

        switch (btok->type) {
            case DSTRING_CLOSE:
            case SSTRING_CLOSE:
                STACK_PUSH(token_new(STRING_LITERAL, strdup(buf)));
                CLEAR_BUFFER();
                break;
            
            case DSTRING_OPEN:
            case SSTRING_OPEN:
                CLEAR_BUFFER();
                break;
            
            case COMMA:
                CLEAR_BUFFER();
                break;
            
            case ARRAY_OPEN:
                vector_append(token_stack, HEAP_DATA(vector_new()));
                break;
            
            case ARRAY_CLOSE:
                data* array = vector_pop(token_stack, VECTOR_LENGTH(token_stack) - 1);
                STACK_PUSH(token_new(ARRAY_LITERAL, data_content(array)));
                break;

            case NOTOK:
                WRITE_BUFFER();
                break;

            case WHITESPACE:
                CLEAR_BUFFER();
                break;
        }
    }

    vector_free(basic);
    //vector_free(token_stack);
    free(buf);

    return tokens;
}