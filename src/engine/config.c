#include <controlmanual/core/error.h> // process_errors
#include <controlmanual/core/map.h>
#include <controlmanual/core/ui.h>
#include <controlmanual/core/util.h> // FAIL, safe_malloc
#include <controlmanual/engine/config.h>
#include <controlmanual/engine/lexer.h> // tokenize
#include <controlmanual/engine/util.h>
#include <controlmanual/engine/main.h>
#include <string.h> // strlen
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

settings cm_settings = {
    false,
    false
};

void load_config(void) {
    char* file = cat_path(
        DIR,
        ".cminit"
    );

    if (!exists(file))
        fclose(
            fopen(
                file,
                "w"
            )
        );

    char* content = read_file(file);

    size_t len = strlen(content);
    char* buf = safe_malloc(len + 1);
    strcpy(
        buf,
        ""
    );

    for (int i = 0; i < len; i++) {
        char c = content[i];
        if (c != ';') {
            char* charstr = char_to_string(c);
            strcat(
                buf,
                charstr
            );
            free(charstr);
            continue;
        }

        command_exec(buf);
        process_errors(false);

        free(buf);
        buf = safe_malloc(len + 1);
        strcpy(
            buf,
            ""
        );
    }

    free(buf);
    free(file);
    free(content);
}
