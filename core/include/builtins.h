#ifndef __CM_BUILTINS_H__
#define __CM_BUILTINS_H__

#include <cmobject.h>

extern type_object* string;
extern type_object* integer;
extern type_object* boolean;

void setup_types(void);
void finalize_types(void);

#endif