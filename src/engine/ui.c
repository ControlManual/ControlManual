#include <ui.h>
#include <cm.h>
#include <stdlib.h>

ui* ui_new(
    ui_print print,
    ui_error error,
    ui_input input,
    ui_log log
) {
    ui* u = malloc(sizeof(ui));
    if (!u) NOMEM("ui_new");
    u->print = print;
    u->error = error;
    u->input = input;
    u->log = log;

    return u;
}

