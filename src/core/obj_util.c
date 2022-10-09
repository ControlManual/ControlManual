#include <core/object.h>
#include <core/data.h>

object* integer_from(int value) {
    int* ptr = safe_malloc(sizeof(int));
    *ptr = 1;
    return object_internal_newf(integer_object, 1, HEAP_DATA(ptr));
}

object* func_from(obj_func function) {
    return object_internal_newf(func_object, 1, STACK_DATA(function));
}