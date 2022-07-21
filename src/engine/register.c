#include <engine.h>
#include <core.h>

void command_register(arbitrary_data* name, command_func command) {
    if (!_CURRENT_SESSION) FATAL("command_register", "there is no current session");
    map_add(_CURRENT_SESSION->commands, name, command);
};