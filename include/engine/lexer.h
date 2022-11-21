#ifndef CM_LEXER_H
#define CM_LEXER_H

#include <core/vector.h>

vector* tokenize(const char* str);

typedef enum ENUM_TOKEN_TYPE {
    STRING_LITERAL,
    ARRAY_LITERAL,
    REFERENCE,
    GROUP_LITERAL,
    INTEGER_LITERAL,
    CALL
} token_type;

typedef struct STRUCT_TOKEN {
    token_type type;
    void* content;
} token;

typedef struct STRUCT_CALLEXPR {
    vector* tokens;
    token* name;
} callexpr;

#endif
