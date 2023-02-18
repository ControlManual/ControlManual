#ifndef CM_DEBUG_H
#define CM_DEBUG_H
#include <controlmanual/core/util.h>

/* NOTE: Debug operations do not have to be supported on windows! */

#ifdef CM_DEBUG
    #include <controlmanual/core/ui.h>
    #include <stdio.h>
    #include <stdarg.h>

    static UNUSED void _print(const char* message) {
        if (!cm_impl_ui_wrapper) {
            puts(message);
        } else {
            cm_impl_ui_wrapper->print(message);
        }
    }

    static UNUSED void _print_format(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        char buf[256];

        vsnprintf(buf, 256, fmt, args);
        va_end(args);

        if (!cm_impl_ui_wrapper) {
            puts(buf);
        } else {
            cm_impl_ui_wrapper->print(buf);
        }
    }

    #define PRINT(str) _print(str)
    #define PRINTSTR(var) _print_format(#var ": %s", var)
    #define PRINTINT(var) _print_format(#var ": %d", var)
    #define PRINTADDR(addr) _print_format(#addr ": %p", addr)

    #ifdef PLATFORM_POSIX
        #include <fcntl.h>
        #include <unistd.h>

        static UNUSED void _debug_address(void* ptr, char* name) {
            int nullfd = open("/dev/random", O_WRONLY);

            if (write(nullfd, ptr, sizeof(ptr)) < 0)
            {
                _print_format("(%s) invalid address: %p", name, ptr);
            }
            close(nullfd);
        }

        #define DEBUG_ADDRESS(ptr) _debug_address(ptr, #ptr)
    #else
        #define DEBUG_ADDRESS(ptr) PRINT("debugging addresses is not supported on windows")
    #endif
#else
    #define DEBUG_ADDRESS(ptr)
    #define PRINT(str)
    #define PRINTSTR(str)
    #define PRINTINT(obj)
    #define PRINTADDR(addr)
#endif

#endif