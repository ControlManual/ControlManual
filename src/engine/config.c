#include <engine/util.h> // PLATFORM_POSIX
#include <core/util.h> // FAIL, safe_malloc
#include <string.h> //strlen
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <engine/lexer.h> // tokenize
#include <core/error.h> // process_errors
#include <core/map.h>
#include <core/ui.h>
#include <engine/config.h>

#ifdef PLATFORM_POSIX
#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#define SEPERATOR '/'
#else
#include <windows.h>
#include <io.h>
#define F_OK 0
#define access _access
#define SEPERATOR '\\'
#endif

bool exists(char* path) {
    return access(path, F_OK) == 0;
}

char* home(void) {
#ifdef PLATFORM_POSIX
    struct passwd* p = getpwuid(getuid());
    if (p) return p->pw_dir;
    char* d = getenv("HOME");
    if (!d) FAIL("failed to acquire home directory");
    return d;
#else
    return getenv("USERPROFILE");
#endif
}

char* cat_path(char* a, char* b) {
    char* result = malloc(strlen(a) + strlen(b) + 2);
    sprintf(result, "%s%c%s", a, SEPERATOR, b);
    return result;
}

void create_dir(char* path) {
#ifdef PLATFORM_POSIX
    if (mkdir(path, 0777) == -1) FAIL(
        "failed to create control manual directory"
    );
#else
    if (!CreateDirectory(path, NULL)) FAIL(
        "failed to create control manual directory"
    ); 
#endif
}

char* read_file(char* path) {
    FILE* f = fopen(path, "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char* string = safe_malloc(fsize + 1);
    fread(string, fsize, 1, f);
    fclose(f);

    string[fsize] = 0;
    return string;
}

config* load_config(void) {
    char* dir = cat_path(home(), ".controlmanual");
    if (!exists(dir)) create_dir(dir);
    char* file = cat_path(dir, ".cmconfig");

    if (!exists(file)) {
        fopen(file, "w");
    }
   
    char* content = read_file(file);

    size_t len = strlen(content);
    char* buf = safe_malloc(len + 1);
    strcpy(buf, "");

    for (int i = 0; i < len; i++) {
        char c = content[i];
        if (c != ';') {
            strcat(buf, char_to_string(c));
            continue;
        }
        
        vector* tokens = tokenize(buf);
        process_errors(true);
        token* key = vector_get(tokens, 0);

        if (VECTOR_LENGTH(tokens) < 2) RETN(
            THROW_STATIC(
                "config item must have at least two items (key and value)",
                "<config>"
            )
        );
        if (key->type != STRING_LITERAL) RETN(
            THROW_STATIC("key must be a string", "<config>")
        );
        ui* u = UI();
        bool found = !strcmp(key->content, "alias") ||
            !strcmp(key->content, "load");

        if (!found) {
            char* tp = safe_malloc(28 + strlen(key->content));
            sprintf(tp, "invalid configuration key: %s", (char*) key->content);
            u->warn(tp);
            free(tp);
        }

        free(buf);
        buf = safe_malloc(len + 1);
        strcpy(buf, "");
    }
    
    free(dir);
    free(file);
    free(content);
}
