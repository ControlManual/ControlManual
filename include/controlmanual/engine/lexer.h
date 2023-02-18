#ifndef CM_LEXER_H
#define CM_LEXER_H

#include <controlmanual/core/vector.h>
#include <controlmanual/core/map.h>
#include <controlmanual/core/data.h>
#include <controlmanual/core/util.h>

vector* tokenize(const char* str);

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
