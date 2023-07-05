/*
    Engine Shell (ESH) 1.0.0
    Reference implementation of Control Manual.
 */
#include <controlmanual/controlmanual.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h> // isdigit
#include <time.h>
#define OVERWRITE_LAST "\033[F"
#define DEFAULT_COLOR "\033[0m"
#define REVERSE_COLOR "\033[7m"
#define UP_ARROW ((char) 66)
#define DOWN_ARROW ((char) 65)
#define CTRL_C ((char) 3)
#define ENTER ((char) 13)
#define ERASE "\33[2K\r"

#ifdef PLATFORM_WIN
#include <conio.h> // getch
#else
#include <unistd.h>
#include <termios.h>
char getch() {
    int c = 0;
    static struct termios old_termios, new_termios;

    tcgetattr(STDIN_FILENO, &old_termios);
    new_termios = old_termios;

    cfmakeraw(&new_termios);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
    c = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &old_termios);
    return (char) c;
}
#define clrscr() printf("\e[1;1H\e[2J")
#endif

typedef struct STRUCT_WINDOW_ENTRY {
    windowid id;
    const char* name;
} window_entry;

void err(error* e, tcontext* tc) {
    char* origin = data_content(e->origin);

    if (tc->state == OBJECT_EXEC) {
        char* ob_name = CONTENT_STR(tc->name);
        char* str = safe_malloc(4 + strlen(ob_name) + strlen(origin));
        sprintf(str, "%s() %s", ob_name, origin);
        origin = str;
    } else if (tc->state == COMMAND_EXEC) {
        origin = CONTENT_STR(tc->name);
    }

    printf("error occured in %s: %s\n", origin, CONTENT_STR(e->content));
    if (e->expr) {
        puts(data_content(e->expr));
        size_t len = strlen(data_content(e->expr));
        for (int x = 0; x < len; x++) {
            bool found = false;
            for (int i = 0; i < VECTOR_LENGTH(e->problems); i++) {
                int* idx = vector_get(e->problems, i);
                if (*idx == x) found = true;
            }

            printf("%c", found ? '^' : ' ');
        }
        puts("");
    }
}

data* inp(input_scope scope, const char* prompt) {
    switch (scope) {
    case GLOBAL_ACCESS:
        // prompt is NULL
        printf("%s >> ", PATH);
        break;
    case COMMAND_ACCESS:
        puts(prompt ? prompt : "");
        break;
    case FUNCTION_ACCESS: {
        tcontext* tc = TC();
        printf("(%s) %s", CONTENT_STR(tc->name), prompt ? prompt : "");
        break;
    }
    }

    char* line = NULL;
    size_t n = 0;
    getline(&line, &n, stdin);
    line[strcspn(line, "\n")] = 0;
    return HEAP_DATA(line);
}

void warn(const char* msg) {
    printf("warning: %s\n", msg);
}

void uiprint(const char* msg, bool newline) {
    if (newline) puts(msg);
    else printf("%s", msg);
}

vector* windows;

void uistart() {
    windows = vector_new();
    puts("ESH 1.0.0");
    puts("Control Manual " VERSION_FULL);
}

void iterfunc(schema* c) {
    printf("%s - %s\n", CONTENT_STR(c->name), CONTENT_STR(c->description));

    for (int i = 0; i < c->params_len; i++) {
        param* p = c->params[i];

        if (p->flag) {
            printf("    --%s, -%s: %s\n", CONTENT_STR(p->name), CONTENT_STR(
                p->shorthand), CONTENT_STR(p->description));
        } else if (p->option) {
            printf("    %s (", CONTENT_STR(p->name));
            for (int i = 0; i < p->options_size; i++) {
                printf("%s", p->options[i]);
                if ((p->options_size - 1) != i) printf("|");
            }
            printf(")\n");
        } else {
            char* df_string;
            if (p->df) {
                df_string = safe_malloc(14 + strlen(data_content(p->df)));
                sprintf(df_string, " [default %s]", CONTENT_STR(p->df));
            } else {
                df_string = safe_malloc(1);
                strcpy(df_string, "");
            }
            char* name_str = safe_malloc(strlen(data_content(p->name)) + 6 +
                (p->shorthand ? strlen(data_content(p->shorthand)) : 0));
            sprintf(
                name_str,
                "%s%s%s%s%s",
                p->keyword ? "--" : "",
                CONTENT_STR(p->name),
                p->shorthand ? ", " : "",
                p->shorthand ? "-" : "",
                p->shorthand ? CONTENT_STR(p->shorthand) : ""
            );

            printf(
                "    %s (%s)%s: %s\n",
                name_str,
                p->tp == &any ? "any" : CONTENT_STR(p->tp->name),
                df_string,
                CONTENT_STR(p->description)
            );

            free(name_str);
            free(df_string);
        }
    }
}

void help(map* m) {
    iter_commands(m, iterfunc);
}

void alert(const char* str) {
    printf("[*] %s\n", str);
}

bool conf(const char* prompt) {
    printf("%s [y/n] ", prompt);
    char c = ' '; // dummy value

    while (c != 'y' && c != 'n' && c != 'Y' && c != 'N') {
        c = getch();
        if (c == (char) 3) c = 'n';
    }

    puts("");
    return c == 'Y' || c == 'y';
}


windowid last_window = 0;

static window_entry* find_entry(windowid id, size_t* index) {
    for (int i = 0; i < VECTOR_LENGTH(windows); i++) {
        window_entry* w = VECTOR_GET(windows, i);
        if (w->id == id) {
            if (index) *index = i;
            return w;
        }
    }
    FAIL("failed to find window entry");
}

windowid uiwindow(const char* name) {
    printf("-- [%s] --\n", name);
    window_entry* entry = safe_malloc(sizeof(window_entry));
    entry->id = ++last_window;
    entry->name = name;
    vector_append(windows, HEAP_DATA(entry));
    return last_window;
}

void uiwindow_close(windowid id) {
    size_t index;
    window_entry* entry = find_entry(id, &index);
    printf("-- [/%s] --\n", entry->name);
    vector_remove(windows, index);
}

void uiwindow_write(windowid id, const char* message) {
    printf("%s\n", message);
}

void clear() {
    clrscr();
}

void uiend() {
    vector_free(windows);
}

static void reload_choices(const char** choices, size_t nchoices, size_t
                           current_choice, bool clear) {
    if (clear) {
        for (int i = 0; i < nchoices; i++) {
            printf(OVERWRITE_LAST);
        }
    }
    for (int i = 0; i < nchoices; i++) {
        if (i == current_choice) printf(REVERSE_COLOR);
        printf("%d). %s" DEFAULT_COLOR "\n", i + 1, choices[i]);
    }
}

size_t choose(const char* prompt, const char** choices, size_t nchoices) {
    puts(prompt);
    size_t current_choice = 0;
    reload_choices(choices, nchoices, 0, false);
    char ch;

    while ((ch = getch())) {
        if (ch == UP_ARROW || ch == 'w') {
            if (current_choice != (nchoices - 1)) {
                ++current_choice;
                reload_choices(choices, nchoices, current_choice, true);
            }
        } else if (ch == DOWN_ARROW || ch == 's') {
            if (current_choice != 0) {
                --current_choice;
                reload_choices(choices, nchoices, current_choice, true);
            }
        } else if (ch == ENTER || ch == CTRL_C) {
            return current_choice;
        }
    }

    return 0;
}

void lst(const char** items, size_t nitems) {
    for (int i = 0; i < nitems; i++) {
        printf("%s    ", items[i]);
        if (i % 4 == 0) puts("");
    }
}

void ui_log(const char* message, const char* funcname) {
    time_t t = time(NULL);
    char* time_str = ctime(&t);
    time_str[strlen(time_str) - 1] = '\0';
    printf("[%s] (%s): %s\n", time_str, funcname, message);
}

int main(void) {
    ui_register(
        err,
        warn,
        uiprint,
        inp,
        uistart,
        uiend,
        help,
        alert,
        conf,
        uiwindow,
        uiwindow_close,
        uiwindow_write,
        clear,
        choose,
        lst,
        ui_log
    );
    start();
    return 0;
}
