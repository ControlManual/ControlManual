#include <controlmanual/core/ui.h>
#include <controlmanual/core/util.h> // safe_malloc
#include <stdlib.h> // NULL
#include <stdio.h> // vsnprintf
#undef UI
#define UI cm_impl_ui_wrapper
#define FMT(ob) { \
    ui* u = ui_acquire(); \
    va_list vargs; \
    va_start(vargs, fmt); \
    char* result = format_va(fmt, vargs); \
    va_end(vargs); \
    if (!result) { \
        process_errors(); \
        return; \
    } \
    u->ob(result); \
    free(result); \
}

ui* UI = NULL;

inline ui* ui_acquire() {
    if (!UI) FAIL("UI has not been registered");
    return UI;
}

void ui_register(
    ui_error error,
    ui_onearg warn,
    ui_onearg print,
    ui_input input,
    ui_none start,
    ui_none end,
    ui_help help,
    ui_onearg alert,
    ui_confirm confirm,
    ui_window window,
    ui_window_close window_close,
    ui_window_write window_write,
    ui_none clear 
) {
    ui* u = safe_malloc(sizeof(ui));
    u->error = error;
    u->warn = warn;
    u->print = print;
    u->input = input;
    u->start = start;
    u->help = help;
    u->end = end;
    u->alert = alert;
    u->confirm = confirm;
    u->window = window;
    u->window_close = window_close;
    u->window_write = window_write;
    u->clear = clear;
    UI = u;
}

void print_obj(object* o) {
    ui* u = ui_acquire();
    object* str = object_to_string(o);
    if (process_errors()) return;
    u->print(STRING_VALUE(str));
}

void print(const char* str) {
    ui* u = ui_acquire();
    u->print(str);
}

void alert(const char* str) {
    ui* u = ui_acquire();
    u->alert(str);
}

void warn(const char* str) {
    ui* u = ui_acquire();
    u->warn(str);
}

data* input(const char* prompt) {
    ui* u = ui_acquire();
    return u->input(COMMAND_ACCESS, prompt);
}

bool confirm(const char* prompt) {
    ui* u = ui_acquire();
    return u->confirm(prompt);
}

windowid window(const char* name) {
    ui* u = ui_acquire();
    return u->window(name);
}

void window_close(windowid id) {
    ui* u = ui_acquire();
    u->window_close(id);
}

void window_write(windowid id, const char* message) {
    ui* u = ui_acquire();
    u->window_write(id, message);
}

void clear() {
    ui* u = ui_acquire();
    u->clear();
}

void print_fmt(const char* fmt, ...) FMT(print)
void alert_fmt(const char* fmt, ...) FMT(alert)
void warn_fmt(const char* fmt, ...) FMT(warn)