#ifndef CM_CONFIG_H
#define CM_CONFIG_H

#include <controlmanual/core/map.h>
#include <controlmanual/core/util.h>

typedef struct STRUCT_CONFIG {
    map* aliases;
} config;

API config* load_config(void);
extern char* cm_dir;

#endif
