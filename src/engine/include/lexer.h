#ifndef __ENGINE_PARSER_H__
#define __ENGINE_PARSER_H__

typedef enum ENUM_TOKENTYPE {
    KEYWORD,
    REF,
} token_type;

typedef struct STRUCT_TOKEN {
    char* data;
    token_type type;
} token;

#endif