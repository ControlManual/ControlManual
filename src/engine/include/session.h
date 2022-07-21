#ifndef __ENGINE_SESSION_H__
#define __ENGINE_SESSION_H__

#include <core.h>

typedef struct STRUCT_SESSION session;

typedef struct STRUCT_UI ui;
typedef void (*ui_outmsg)(session*, arbitrary_data* msg);
typedef arbitrary_data* (*ui_input)(session*, arbitrary_data* prompt);
typedef void (*ui_event)(session*);

struct STRUCT_UI {
    ui_outmsg print;
    ui_outmsg error;
    ui_input input;
    ui_outmsg log;
    ui_event start;
    ui_event end;
};

struct STRUCT_SESSION {
    scope* global;
    ui* engine_ui;
    map* commands;
    char* path;
    scope* current_scope;
};

ui* ui_new(
    ui_outmsg print,
    ui_outmsg error,
    ui_input input,
    ui_outmsg log,
    ui_event start,
    ui_event end
);

session* session_new(ui* engine_ui);
void session_free(session* ses);

#endif