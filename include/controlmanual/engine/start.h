#ifndef CM_START_H
#define CM_START_H
#include <controlmanual/core/object.h>
#include <controlmanual/engine/util.h>
#define PATH cm_current_path

void start();

extern char* cm_current_path;
void command_exec(char* str);
void unload();

#endif