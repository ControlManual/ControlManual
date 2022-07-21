#include <engine.h>
#include <core.h>
#include <stdbool.h>
#include <stdio.h>
#ifdef _WIN32
#include <windows.h>
#define LOAD LoadLibrary
#define GET GetProcAddress
#define CLOSE FreeLibrary
#else
#include <dlfcn.h>
#define LOAD dlopen
#define GET dlsym
#define CLOSE dlclose
#endif


void finalize() {
    finalize_types();
}

char* conf_path = NULL;
char* comm_path = NULL;

static void _loop(session* restrict s) {
    while (true) {
        char* input = s->engine_ui->input(s, ">> ");
    }
}

static void _init_config(void) {
    if (!conf_path) conf_path = (char*) join_paths(home(), ".controlmanual");
    if (!comm_path) conf_path = (char*) join_paths(conf_path, "commands");
    if (!directory_exists(conf_path)) {
        create_directory(conf_path);
        create_directory(comm_path);
    }
}

static void _init_commands(session* ses) {
}


void boot(ui* engine_ui) {
    _init_config();
    setup_types();
    session* ses = session_new(engine_ui);
    _CURRENT_SESSION = ses;
    _init_commands(ses);
    engine_ui->start(ses);
    _loop(ses);
}