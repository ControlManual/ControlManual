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
    ui_print print,
    ui_input input,
    ui_none start,
    ui_none end,
    ui_help help,
    ui_onearg alert,
    ui_confirm confirm,
    ui_window window,
    ui_window_close window_close,
    ui_window_write window_write,
    ui_none clear,
    ui_choose choose,
    ui_list list,
    ui_twoargs log
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
    u->choose = choose;
    u->list = list;
    u->log = log;
    UI = u;
}

void print_obj(object* o) {
    ui* u = ui_acquire();
    object* str = object_to_string(o);
    if (process_errors()) return;
    u->print(STRING_VALUE(str), true);
}

void print(const char* str) {
    ui* u = ui_acquire();
    u->print(str, true);
}

void print_noline(const char* str) {
    ui* u = ui_acquire();
    u->print(str, false);
};

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

void write_log(const char* message, const char* funcname) {
    ui* u = ui_acquire();
    u->log(message, funcname);
}

void print_fmt(const char* fmt, ...) {
    ui* u = ui_acquire();
    va_list vargs;
    va_start(vargs, fmt);
    char* result = format_va(fmt, vargs);
    va_end(vargs);
    if (!result) {
        process_errors();
        return;
    }
    u->print(result, true);
    free(result);
}
void alert_fmt(const char* fmt, ...) FMT(alert)
void warn_fmt(const char* fmt, ...) FMT(warn)

inline size_t choose(const char* name, const char** choices, size_t nchoices) {
    ui* u = ui_acquire();
    return u->choose(name, choices, nchoices);
}

size_t choosef(const char* name, size_t nargs, ...) {
    ui* u = ui_acquire();
    char** choices = safe_calloc(nargs, sizeof(char*));
    va_list vargs;
    va_start(vargs, nargs);

    for (int i = 0; i < nargs; i++)
        choices[i] = va_arg(vargs, char*);

    size_t res = u->choose(name, (const char**) choices, nargs);
    free(choices);
    return res;
}
