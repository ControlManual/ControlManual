#include <core/ui.h>
#include <core/util.h> // safe_malloc
#include <stdlib.h> // NULL
#undef UI
#define UI cm_impl_ui_wrapper

ui* UI = NULL;

inline ui* ui_acquire() {
    if (!UI) FAIL("UI has not been registered");
    return UI;
}

void ui_register(
    ui_error error,
    ui_onearg warn,
    ui_twoargs log,
    ui_twoargs notify,
    ui_onearg print,
    ui_input input,
    ui_none start,
    ui_none end,
    ui_help help
) {
    ui* u = safe_malloc(sizeof(ui));
    u->error = error;
    u->warn = warn;
    u->log = log;
    u->notify = notify;
    u->print = print;
    u->input = input;
    u->start = start;
    u->help = help;
    u->end = end;
    UI = u;
}
