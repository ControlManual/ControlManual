#ifndef CM_CONFIG_H
#define CM_CONFIG_H

#include <controlmanual/core/map.h>
#include <controlmanual/core/util.h>

typedef struct STRUCT_SETTINGS {
    bool use_ui_options;
    bool ctrl_c_kill;
} settings;

API void load_config(void);
extern settings cm_settings;

#define SETTINGS cm_settings

#endif
