#ifndef CM_ENGINE_UTIL_H
#define CM_ENGINE_UTIL_H
#include <controlmanual/core/util.h>
#include <stdbool.h>
#include <stdint.h>

API char* char_to_string(const char c);
API bool exists(const char* path);
API char* home(void);
API char* cat_path(const char* a, const char* b);
API void create_dir(const char* path);
API char* read_file(const char* path);

typedef FUNCTYPE(dir_iter_func, void, (char*));

#define COMMAND_NAME( \
    value) EXPORT data* cm_command_name(void) { return STACK_DATA(#value); }
#define COMMAND_DESCRIPTION( \
    value) \
    EXPORT data* cm_command_description(void) { return STACK_DATA(value); }
#define COMMAND_PARAMS(...) EXPORT paramcontext* cm_param_construct(void) { \
    return paramcontext_new(param_array_from((param*[]) { __VA_ARGS__ }, NUMARGS(__VA_ARGS__)), NUMARGS(__VA_ARGS__)); \
}
#define COMMAND EXPORT object* cm_command_caller
#define COMMAND_INIT EXPORT void cm_command_init

#define PLUGIN_NAME( \
    value) EXPORT data* cm_plugin_name(void) { return STACK_DATA(#value); }
#define PLUGIN EXPORT void cm_plugin_func

#define MIDDLEWARE EXPORT void cm_middleware_func

#define PACKAGE_NAME( \
    name) EXPORT data* cm_package_name(void) { return STACK_DATA(#name); }
#define PACKAGE_DESCRIPTION( \
    value) \
    EXPORT data* cm_package_description(void) { return STACK_DATA(#value); }
#define PACKAGE_LIST(...)
#define PACKAGE_COMMAND(name, desc, ...)

#define IS_COMMAND (1UL << 1)
#define IS_PLUGIN (1UL << 2)
#define IS_MIDDLEWARE (1UL << 3)
#define FLAGS(flags) EXPORT size_t cm_symbol_flags(void) { \
    return (0) | (flags); \
}

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

API bool iterate_dir(const char* path, dir_iter_func func);
API bool is_file(const char* path);

#ifdef CM_LARGE_OPTION_SIZE
typedef uint16_t option;
#else
typedef uint8_t option;
#endif


#endif
