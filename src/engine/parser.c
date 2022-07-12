#include <cm.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#define ITEMALLOC(item) if (!list) { \
  list = linked_list_new(item); \
  first = list; \
} else list = linked_list_append(list, item);
#define OPERATOR(op) if (last == op) { \
    char* item = (char*) malloc(3 * sizeof(char)); \
    sprintf(item, "%c%c", last, c); \
    int buf_len = strlen(buffer); \
    char* buf = (char*) malloc(buf_len * sizeof(char)); \
    strncpy(buf, buffer, buf_len - 1); \
    ITEMALLOC(buf); \
    list = linked_list_append(list, item); \
    strcpy(buffer, ""); \
  }
// i probably shouldnt use a macro for this but who cares
node* tokenize(const char* data) {
    node* first = NULL;
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
                char* item = malloc((strlen(buffer) + 1) * sizeof(char));
                strcpy(item, buffer);
                if (!squote && !dquote && !parens && !braces)
                  ITEMALLOC(item); 
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

            case '=':
                OPERATOR('=');
                OPERATOR('!');
                OPERATOR('+');
                OPERATOR('-');
                OPERATOR('/');
                OPERATOR('*');
                break;

            default:
                strncat(buffer, &c, 1);
        }
        last = c;
    }
    buffer = realloc(buffer, (strlen(buffer) + 1) * sizeof(char));
    linked_list_append(list, buffer);
    return first;
}
