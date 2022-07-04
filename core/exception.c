#include <exception.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

exc* _CURRENT_ERROR = NULL;

exc* error_new(
    const char* message,
    const char* what
) {
    exc* exception = (exc*) malloc(sizeof(exc));
    if (!exception) {
        puts("failed to generate error: out of memory");
        exit(1);
    }

    exception->what = what;
    exception->message = message;
    return exception;
}

void process_error(void) {
    if (_CURRENT_ERROR) {
        fprintf(
            stderr,
            " --- CORE ERROR --- \n%s() generated exception with message: %s\n",
            _CURRENT_ERROR->what,
            _CURRENT_ERROR->message
        );
        free(_CURRENT_ERROR);
        exit(1);
    }
}

exc* catch_error(void) {
    exc* caught = (exc*) malloc(sizeof(exc));

    if (!caught) {
        puts("failed to catch error: out of memory");
        exit(1);
    }

    memcpy(caught, _CURRENT_ERROR, sizeof(exc));
    free(_CURRENT_ERROR);
    _CURRENT_ERROR = NULL;
    return caught;
}