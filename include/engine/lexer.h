#ifndef CM_LEXER_H
#define CM_LEXER_H

#include <core/list.h>

typedef enum ENUM_TOKEN_TYPE {
    COMMAND,
    STRING_LITERAL,
    INT_LITERAL,
    ARRAY_LITERAL,
    REFERENCE,
} token_type;

typedef struct STRUCT_TOKEN {
    token_type type;
    list* tokens;
    void* content;
} token;

#endif