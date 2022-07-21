#include <engine.h>
#include <core.h>
#include <stdbool.h>
#include <stdio.h>
#ifdef _WIN32
#include <windows.h>
#define LOAD(name) LoadLibrary(name)
#define GET(handle, name) GetProcAddress(handle, name)
#define CLOSE(handle) FreeLibrary(handle)
#else
#include <dlfcn.h>
#define LOAD(name) dlopen(name, RTLD_LAZY)
#define GET(handle, name) dlsym(handle, name)
#define CLOSE(handle) dlclose(handle)
#endif
#include <string.h>

char* conf_path = NULL;
char* comm_path = NULL;

static char* _home() {
    #ifdef _WIN32
        return getenv("HOMEPATH");
    #else
        return getenv("HOME");
    #endif
}

static void _loop(session* restrict s) {
    while (true) {
        arbitrary_data* input = s->engine_ui->input(s, STACK_DATA(">> "));

        if (!strcmp(input->contents, "exit")) {
            data_free(input);
            break;
        }

        data_free(input);
    }
}

static void _init_config(void) {
    if (!conf_path) conf_path = (char*) join_paths(_home(), ".controlmanual");
    if (!comm_path) comm_path = (char*) join_paths(conf_path, "commands");
    if (!directory_exists(conf_path)) {
        create_directory(conf_path);
        create_directory(comm_path);
    }
}

static void _command_loader(session* ses, char* file) {
    void* dll = LOAD(file);
    if (!dll) {
        char* string = malloc((sizeof(char) * STRSIZE(file)) + 25);
        sprintf(string, "failed to load dll at \"%s\"", file);
        ses->engine_ui->error(ses, HEAP_DATA(string));
    }
}

static void _init_commands(session* ses) {
    NONULL(comm_path, "_init_commands");
    directory_iter_files(comm_path, &_command_loader, ses);
}

static void _end(session* ses) {
    free(comm_path);
    comm_path = NULL;
    free(conf_path);
    conf_path = NULL;

    ses->engine_ui->end(ses);

    free(ses->engine_ui);
    session_free(ses);
    finalize_types();
}


void boot(ui* engine_ui) {
    _init_config();
    setup_types();
    session* ses = session_new(engine_ui);
    _init_commands(ses);
    engine_ui->start(ses);
    _loop(ses);
    _end(ses);
}