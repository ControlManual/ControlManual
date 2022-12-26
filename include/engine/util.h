#ifndef CM_ENGINE_UTIL_H
#define CM_ENGINE_UTIL_H

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#define PLATFORM_WIN
#endif

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#define PLATFORM_POSIX
#endif

#if !defined(PLATFORM_POSIX) && !defined(PLATFORM_WIN)
#error "this platform is not supported"
#endif

char* char_to_string(char c);

#define COMMAND_NAME(value) const char* cm_command_name = #value;
#define COMMAND_DESCRIPTION(value) const char* cm_command_description = value;
#define COMMAND_PARAMS(...) param** cm_param_construct(void) { \
    return param_array_from((param*[]) { __VA_ARGS__ }); \
}
#define COMMAND void cm_command_caller

#endif
