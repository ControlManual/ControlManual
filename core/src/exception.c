#include <exception.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

exc* _CURRENT_ERROR = NULL;
bool _SUPPRESS = false;

/* Generate a new error. */
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

/* Process the current error if it exists or isn't suppressed. */
void process_error(void) {
    if (_CURRENT_ERROR && !_SUPPRESS) {
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