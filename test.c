#include <core/data.h>
#include <core/map.h>
#include <core/object.h>
#include <core/error.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main() {
    ERRSTACK_INIT;
    init_types();
    int* a = safe_malloc(sizeof(int));
    *a = 1;
    object* one = object_internal_newf(object_from(&integer), 1, HEAP_DATA(a));
    object_newf(object_from(&func), 1, NOFREE_DATA(one));
    unload_types();
    ERRSTACK_FREE;
    return 0;
}
