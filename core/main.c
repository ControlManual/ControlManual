#include <cm.h>
#include <stdlib.h>
#include <stdio.h>

void test_init(vector* args, instance_object* self) {
    printf("type name: %s\n", self->type->name);
}

instance_object* test_alloc(type_object* cls) {
    instance_object* obj = (instance_object*) malloc(sizeof(instance_object));
    if (!obj) NOMEM("test_alloc");
    puts("allocated");
    return obj;
}

void test_dealloc(instance_object* self) {
    puts("freeing");
}

int main() {
    type_object* test = type_new("test", map_new(), &test_init, &test_alloc, &test_dealloc);
    instance_object* obj = instance_construct(test, vector_new());
    instance_free(obj);
    type_free(test);
    return 0;
}