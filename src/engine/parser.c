#include <cm.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <parser.h>
#include <regex.h>

token* token_new(char* restrict content, token_type type) {
    token* t = (token*) malloc(sizeof(token));
    t->content = content;
    t->type = type;
    return t;    
}

inline void token_free(token* restrict tok) {
    free(tok->content);
    free(tok);
}

node** split(const char* restrict data) {
    node* list = NULL;
    node** first = NULL;

    int parens = 0;
    int braces = 0;
    char last = ' ';
    bool squote = false;
    bool dquote = false;
    char* buffer = (char*) malloc((strlen(data) + 1) * sizeof(char));


    for (int i = 0; i < strlen(data); i++) {
        const char c = data[i];
        
        switch (c) {
            case '{':
                braces++;
                break
            
            case '}':
                braces--;
                break;

            case '(':
                parens++;
                break;
            
            case ')':
                parens--;
                break;
            
            case '"':
                dquote = !dquote;
                break;
            
            case '\'':
                squote = !squote;
                break;
            
            case ' ':
                if (!squote && !dquote && !parens && !braces) {
                    char* item = (char*) malloc((strlen(buffer) + 1) * sizeof(char));
                    strcpy(item, buffer);
                    if (!list) {
                        list = linked_list_new(item);
                        first = &list;
                    }
                    else list = linked_list_append(list, item);
                    strcpy(buffer, "");
                }
                break;
        }
    }

    return first;
}

node* tokenize(const char* restrict data) {
    node* list = *split(data);
    
    for (int i = 0; i < *list->size; i++) {
        
    }
}