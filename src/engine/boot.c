#include <engine.h>
#include <core.h>


void finalize() {
    finalize_types();
}

void boot(ui* engine_ui) {
    setup_types();
    session* ses = session_new(engine_ui);

    engine_ui->start(ses);
}