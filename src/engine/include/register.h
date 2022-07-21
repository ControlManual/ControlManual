#ifndef __ENGINE_REGISTER_H__
#define __ENGINE_REGISTER_H__

#include <core.h>
#include <engine.h>

typedef arbitrary_data* (*command_func)(session*, vector* args);
void command_register(arbitrary_data* name, command_func command);

#endif