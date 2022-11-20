#ifndef CM_UI_H
#define CM_UI_H

#include <core/util.h>
#include <core/data.h>
#include <core/error.h>

typedef FUNCTYPE(ui_onearg, void, (data*));
typedef FUNCTYPE(ui_twoargs, void, (data*, data*));

typedef struct STRUCT_UI {
    ui_error error;
    ui_twoargs warn;
    ui_twoargs log;
    ui_twoargs notify;
    ui_onearg print;
} ui;

ui* ui_new(
    ui_error error,
    ui_twoargs warn,
    ui_twoargs log,
    ui_twoargs notify,
    ui_onearg print
);

#endif