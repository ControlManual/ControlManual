#include <core/data.h>
#include <core/map.h>
#include <core/object.h>
#include <core/error.h>
#include <core/vector.h>
#include <core/obj_util.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

object* my_callable(object* o, vector* v) {
    puts("hello, world!");
    return NULL;
}

int main() {
    ERRSTACK_INIT;
    init_types();
    object* o = func_from(my_callable);
    object_callf(o, 0);
    unload_types();
    ERRSTACK_FREE;
    return 0;
}
