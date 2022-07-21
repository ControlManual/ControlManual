#include <session.h>
#include <core.h>

session* session_new(ui* restrict engine_ui) {
    session* ses = malloc(sizeof(session));
    if (!ses) NOMEM("session_new");

    ses->commands = map_new();
    ses->global = scope_new(NULL);
    ses->engine_ui = engine_ui;
    ses->current_scope = NULL;

    return ses;
}

void session_free(session* restrict ses) {
    scope_free(ses->global);
    map_free(ses->commands);
}

ui* ui_new(
    ui_outmsg print,
    ui_outmsg error,
    ui_input input,
    ui_outmsg log,
    ui_event start,
    ui_event end
) {
    ui* u = malloc(sizeof(ui));
    if (!u) NOMEM("ui_new");
    u->print = print;
    u->error = error;
    u->input = input;
    u->log = log;
    u->start = start;
    u->end = end;

    return u;
}
