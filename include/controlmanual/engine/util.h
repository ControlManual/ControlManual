#ifndef CM_ENGINE_UTIL_H
#define CM_ENGINE_UTIL_H
#include <controlmanual/engine/commands.h>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#define PLATFORM_WIN
#endif

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#define PLATFORM_POSIX
#endif

#if !defined(PLATFORM_POSIX) && !defined(PLATFORM_WIN)
#error "this platform is not supported"
#endif

#include <stdbool.h>

char* char_to_string(char c);
bool exists(char* path);
char* home(void);
char* cat_path(char* a, char* b);
void create_dir(char* path);
char* read_file(char* path);

#define COMMAND_NAME(value) char* cm_command_name(void) { return #value; }
#define COMMAND_DESCRIPTION(value) char* cm_command_description(void) { return value; }
#define COMMAND_PARAMS(...) paramcontext* cm_param_construct(void) { \
    return paramcontext_new(param_array_from((param*[]) { __VA_ARGS__ }, NUMARGS(__VA_ARGS__)), NUMARGS(__VA_ARGS__)); \
}
#define COMMAND object* cm_command_caller

#ifdef PLATFORM_POSIX
#include <dlfcn.h>
#include <unistd.h>
#include <sys/types.h>
#define OPEN_LIB(path) dlopen(path, RTLD_LAZY)
#define GET_SYMBOL(handle, sym) dlsym(handle, sym)
#define CLOSE_LIB(handle) dlclose(handle)
typedef void* library;
#else
#include <windows.h>
#define OPEN_LIB(path) LoadLibrary(path)
#define GET_SYMBOL(handle, sym) GetProcAddress(handle, sym)
#define CLOSE_LIB(handle) FreeLibrary(handle)
typedef HINSTANCE library;
#endif

bool iterate_dir(char* path, dir_iter_func func);
bool is_file(char* path);

#endif
