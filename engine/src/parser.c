#include <cm.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

node* tokenize(const char* data) {
    node* list = NULL;
    int len = strlen(data);
    char* buffer = malloc((len * sizeof(char)) + 1);
    int parens = 0;
    int braces = 0;
    bool squote = false;
    bool dquote = false;
    char last = ' ';

    for (int i = 0; i < len; i++) {
        char c = data[i];
        switch (c) {
            case ' ':
                char* item = malloc((strlen(buffer) * sizeof(char)) + 1);
                strcpy(item, buffer);
                if (!squote && !dquote && !parens && !braces)
                    if (!list) list = linked_list_new(item);
                    else list = linked_list_append(list, item);
                
                strcpy(buffer, "");
                break;

            case '"':
                dquote = !dquote;
                break;
            
            case '\'':
                squote = !squote;
                break;
            
            case '(':
                parens++;
                break;

            case ')':
                parens--;
                break;
            
            case '{':
                braces++;
                break;
            
            case '}':
                braces--;
                break;

            default:
                strncat(buffer, &c, 1);
        }
        last = c;
    }

    return list;
}