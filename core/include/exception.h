#ifndef __CM_ERROR_H__
#define __CM_ERROR_H__
#include <stdlib.h>
#define THROW(what, message, fatal) _CURRENT_ERROR = error_new(message, what); if (fatal) process_error()
#define FATAL(what, message) THROW(what, message, 1)
#define NOMEM(what) FATAL(what, "out of memory")
#define NONULL(obj, what) if (obj == NULL) FATAL(what, "received NULL pointer")

typedef struct STRUCT_ERROR {
    const char* message;
    const char* what;
} exc;

extern exc* _CURRENT_ERROR;

void process_error(void);

exc* error_new(
    const char* message,
    const char* what
);

exc* catch_error(void);

#endif