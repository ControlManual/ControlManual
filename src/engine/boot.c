#include <engine.h>
#include <core.h>
#include <stdbool.h>
#include <files.h>
#include <stdio.h>

void finalize() {
    finalize_types();
}

char* conf_path = NULL;

static void _loop(session* restrict s) {
    while (true) {
        s->engine_ui->input(s, ">>");
    }
}

static void _init_config(void) {
    if (!conf_path) conf_path = (char*) join_paths(home(), ".controlmanual");
    if (!directory_exists(conf_path)) {
        create_directory(conf_path);
        //create_file(join_paths(conf_path, "test.txt"));
    }
}


void boot(ui* engine_ui) {
    _init_config();
    setup_types();
    session* ses = session_new(engine_ui);

    engine_ui->start(ses);
    _loop(ses);
}