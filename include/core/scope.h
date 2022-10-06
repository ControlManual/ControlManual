#ifndef CM_SCOPE_H
#define CM_SCOPE_H

#include "map.h"

typedef struct STRUCT_SCOPE {
    map* globals;
    map* locals;
} scope;

#endif
