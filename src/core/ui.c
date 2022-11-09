#include <core/ui.h>
#include <core/util.h> // safe_malloc

ui* ui_new(
    ui_error error,
    ui_twoargs warn,
    ui_twoargs log,
    ui_twoargs notify,
    ui_onearg print
) {
    ui* u = safe_malloc(sizeof(ui));
    u->error = error;
    u->warn = warn;
    u->log = log;
    u->notify = notify;
    u->print = print;
    return u;
}