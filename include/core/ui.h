#ifndef CM_UI_H
#define CM_UI_H

#include <core/util.h>
#include <core/data.h>
#include <core/error.h>
#include <engine/commands.h>
#define UI() ui_acquire();

typedef FUNCTYPE(ui_onearg, void, (char*));
typedef FUNCTYPE(ui_twoargs, void, (char*, char*));
typedef FUNCTYPE(ui_input, char*, ());
typedef FUNCTYPE(ui_none, void, ());
typedef FUNCTYPE(ui_help, void, (map*))

typedef struct STRUCT_UI {
    ui_error error;
    ui_onearg warn;
    ui_twoargs log;
    ui_twoargs notify;
    ui_onearg print;
    ui_input input;
    ui_none start;
    ui_none end;
    ui_help help;
} ui;

extern ui* _UI;

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
);

extern ui* ui_acquire();

#endif
