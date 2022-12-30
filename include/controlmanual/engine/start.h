#ifndef CM_START_H
#define CM_START_H
#include <controlmanual/core/object.h>
#include <controlmanual/engine/util.h>
#define PATH cm_current_path
#define GLOBAL cm_global_scope

void start();

extern char* cm_current_path;
extern scope* cm_global_scope;
void command_exec(char* str);
void unload();

#endif