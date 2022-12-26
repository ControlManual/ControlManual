#ifndef CM_START_H
#define CM_START_H
#include <core/object.h>
#include <engine/util.h>
#define PATH cm_current_path
#define GLOBAL cm_global_scope

void start();

extern char* cm_current_path;
extern scope* cm_global_scope;

#endif