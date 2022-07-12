#ifndef __ENGINE_PARSER_H__
#define __ENGINE_PARSER_H__

#include <cm.h>

typedef struct STRUCT_PARSED {
    char* command;
    vector* args;
    map* kw_args;
    vector* flags;
} parsed;

node* tokenize(const char* data);

#endif