#ifndef CM_CONFIG_H
#define CM_CONFIG_H

#include <core/map.h>

typedef struct STRUCT_CONFIG {
    map* aliases;
} config;

config* load_config(void);
extern char* cm_dir;

#endif
