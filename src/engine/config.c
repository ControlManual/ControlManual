#include <controlmanual/engine/lexer.h> // tokenize
#include <controlmanual/core/error.h> // process_errors
#include <controlmanual/core/map.h>
#include <controlmanual/core/ui.h>
#include <controlmanual/engine/config.h>
#include <controlmanual/engine/util.h>
#include <controlmanual/core/util.h> // FAIL, safe_malloc
#include <string.h> // strlen
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

char* cm_dir;

config* load_config(void) {
    cm_dir = cat_path(home(), ".controlmanual");
    if (!exists(cm_dir)) create_dir(cm_dir);
    char* file = cat_path(cm_dir, ".cmconfig");

    if (!exists(file))
        fclose(fopen(file, "w"));

    char* content = read_file(file);

    size_t len = strlen(content);
    char* buf = safe_malloc(len + 1);
    strcpy(buf, "");

    for (int i = 0; i < len; i++) {
        char c = content[i];
        if (c != ';') {
            char* charstr = char_to_string(c);
            strcat(buf, charstr);
            free(charstr);
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
        vector_free(tokens);
    }

    free(buf);
    free(file);
    free(content);
}
