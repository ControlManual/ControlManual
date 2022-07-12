#ifndef __ENGINE_PARSER_H__
#define __ENGINE_PARSER_H__

#include <cm.h>

typedef enum ENUM_TOKENTYPE {
    OPERATOR,
    DATA
} token_type;

typedef struct STRUCT_TOKEN {
    char* content;
    token_type type;
} token;

typedef struct STRUCT_PARSED {
    char* command;
    vector* args;
    map* kw_args;
    vector* flags;
} parsed;

node* tokenize(const char* data);
token* token_new(char* restrict content, token_type type);
extern void token_free(token* restrict tok);

#endif