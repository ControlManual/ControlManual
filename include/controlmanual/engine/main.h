#ifndef CM_MAIN_H
#define CM_MAIN_H
#include <controlmanual/core/object.h>
#include <controlmanual/engine/util.h>
#include <controlmanual/core/util.h>
#define PATH cm_current_path

API void start();

extern char* cm_current_path;
API object* command_exec(const char* str);
API void unload();

#endif