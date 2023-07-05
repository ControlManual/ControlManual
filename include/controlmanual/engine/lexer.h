#ifndef CM_LEXER_H
#define CM_LEXER_H

#include <controlmanual/core/vector.h>
#include <controlmanual/core/map.h>
#include <controlmanual/core/data.h>
#include <controlmanual/core/util.h>

vector* tokenize_basic(const char* str);
vector* tokenize(const char* str);

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
    FLAGC,
} btoken_type;

typedef struct STRUCT_BTOKEN {
    btoken_type type;
    char* content;
    size_t index;
} btoken;

typedef enum ENUM_TOKEN_TYPE {
    STRING_LITERAL,
    ARRAY_LITERAL,
    REFERENCE,
    GROUP_LITERAL,
    INTEGER_LITERAL,
    CALL,
    SFLAG_NVAL,
    KFLAG_NVAL,
    SFLAG,
    KFLAG
} token_type;

typedef struct STRUCT_TOKEN {
    token_type type;
    void* content;
} token;

typedef struct STRUCT_CALLEXPR {
    vector* tokens;
    token* name;
} callexpr;

typedef struct STRUCT_FLAGEXPR {
    char* name;
    data* value;
} flagexpr;

API void params_from_tokens(
    vector* tokens,
    data** command_name,
    vector* params,
    vector* flags,
    map* keywords
);

#endif
