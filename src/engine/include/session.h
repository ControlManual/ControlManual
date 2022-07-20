#ifndef __ENGINE_SESSION_H__
#define __ENGINE_SESSION_H__

#include <core.h>

typedef struct STRUCT_SESSION {
    scope* glbl;
    ui* engine_ui;
    map* commands;
} session;

typedef struct STRUCT_UI ui;
typedef void (*ui_outmsg)(session*, char* msg);
typedef char* (*ui_input)(session*, char* prompt);
typedef void (*ui_start)(session*);

struct STRUCT_UI {
    ui_outmsg print;
    ui_outmsg error;
    ui_input input;
    ui_outmsg log;
    ui_start start;
};

ui* ui_new(
    ui_outmsg print,
    ui_outmsg error,
    ui_input input,
    ui_outmsg log,
    ui_start start
);

session* session_new(ui* restrict engine_ui);
void session_free(session* restrict ses);

#endif