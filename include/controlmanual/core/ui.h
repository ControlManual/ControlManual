#ifndef CM_UI_H
#define CM_UI_H

#include <controlmanual/core/util.h>
#include <controlmanual/core/data.h>
#include <controlmanual/core/error.h>
#include <controlmanual/engine/loader.h>
#define UI() ui_acquire();
#define LOG(msg) write_log(msg, __func__)

typedef enum ENUM_INPUT_SCOPE {
    GLOBAL_ACCESS,
    COMMAND_ACCESS,
    FUNCTION_ACCESS
} input_scope;

typedef size_t windowid;

typedef FUNCTYPE(ui_onearg, void, (const char*));
typedef FUNCTYPE(ui_print, void, (const char*, bool));
typedef FUNCTYPE(ui_twoargs, void, (const char*, const char*));
typedef FUNCTYPE(ui_input, data*, (input_scope, const char*));
typedef FUNCTYPE(ui_none, void, ());
typedef FUNCTYPE (ui_help, void, (map*))
typedef FUNCTYPE (ui_confirm, bool, (const char*))
typedef FUNCTYPE(ui_window, windowid, (const char*));
typedef FUNCTYPE(ui_window_close, void, (windowid));
typedef FUNCTYPE(ui_window_write, void, (windowid, const char*));
typedef FUNCTYPE (ui_choose, size_t, (const char*, const char*[], size_t))
typedef FUNCTYPE(ui_list, void, (const char*[], size_t))

typedef struct STRUCT_UI {
    ui_error error;
    ui_onearg warn;
    ui_print print;
    ui_input input;
    ui_none start;
    ui_none end;
    ui_help help;
    ui_onearg alert;
    ui_confirm confirm;
    ui_window window;
    ui_window_close window_close;
    ui_window_write window_write;
    ui_none clear;
    ui_choose choose;
    ui_list list;
    ui_twoargs log;
} ui;

extern ui* cm_impl_ui_wrapper;

API void ui_register(
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
);

API extern ui* ui_acquire();

API void print_obj(object* o);
API void print(const char* str);
API void alert(const char* str);
API data* input(const char* prompt);
API bool confirm(const char* prompt);
API windowid window(const char* name);
API void window_close(windowid id);
API void window_write(windowid id, const char* message);
API void clear();
API void write_log(const char* message, const char* funcname);
API void print_fmt(const char* fmt, ...);
API void alert_fmt(const char* fmt, ...);
API void warn_fmt(const char* fmt, ...);
API void print_noline(const char* str);

#endif
