#ifndef __CM_ERROR_H__
#define __CM_ERROR_H__
#include <stdlib.h>
#include <stdbool.h>
#define THROW(what, message, fatal) _CURRENT_ERROR = error_new(message, what); if (fatal) process_error()
#define FATAL(what, message) THROW(what, message, 1)
#define NOMEM(what) FATAL(what, "out of memory")
#define NONULL(obj, what) if (obj == NULL) FATAL(what, "received NULL pointer")
#define SET_ERROR(name) exc* name = _CURRENT_ERROR
#define CATCH_ERROR SET_ERROR(exc)
#define PEND_ERROR _SUPPRESS = true
#define UNLOCK_ERROR _SUPPRESS = false

typedef struct STRUCT_ERROR {
    const char* message;
    const char* what;
} exc;

extern exc* _CURRENT_ERROR;
extern bool _SUPPRESS;

void process_error(void);

exc* error_new(
    const char* message,
    const char* what
);

#endif