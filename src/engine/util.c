#include <engine/util.h>
#include <core/util.h> // safe_malloc

char* char_to_string(char c) {
    char* str = safe_malloc(2);
    str[0] = c;
    str[1] = '\0';
    return str;
}
