#ifndef __ENGINE_UI_H__
#define __ENGINE_UI_H__

typedef struct STRUCT_UI ui;
typedef void (*ui_error)(ui*, const char* msg);
typedef void (*ui_print)(ui*, const char* msg);
typedef char* (*ui_input)(ui*, const char* prompt);
typedef void (*ui_log)(ui*, const char* msg);

struct STRUCT_UI {
    ui_print print;
    ui_error error;
    ui_input input;
    ui_log log;
};

ui* ui_new(
    ui_print print,
    ui_error error,
    ui_input input,
    ui_log log
);

#endif