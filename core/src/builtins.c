#include <cm.h>
#include <string.h>
#include <stdio.h>
#define SETUP_TYPE(name, str, init, to_str) name = type_new(str, map_new(), &init, &basic_alloc, NULL, &to_str, NULL)
#define TOSTR(fmt) int len = snprintf(NULL, 0, fmt, self->private); \
    char* str = malloc(len + 1); \
    snprintf(str, len + 1, fmt, self->private); \
    return str

/* Baic allocation function for a type object. */
instance_object* basic_alloc(type_object* type) {
    instance_object* obj = (instance_object*) malloc(sizeof(instance_object));
    if (!obj) NOMEM("basic_alloc");
    return obj;
}

/* To-string method for the string type object. */
char* str_to_string(instance_object* self) {
    char* str = (char*) malloc(
        (strlen(self->type->name) * sizeof(char)) + 
        (sizeof(char) * 5) + 
        (strlen((char*) self->private) * sizeof(char))
    );
    sprintf(str, "%s(\"%s\")", self->type->name, (char*) self->private);
    return str;
}

/* To-string method for the integer type object. */
char* int_to_string(instance_object* self) {
    TOSTR("%d");
}

/* Initialization method for the string type object. */
void string_init(instance_object* self, vector* args) {
    if (args->items[0] == NULL) {
        THROW("string_init", "expected at least 1 value in args", 0);
        return;
    }
    self->private = malloc((strlen(args->items[0]) * sizeof(char)) + 1);
    strcpy(self->private, args->items[0]);
}

/* Deallocation method for the string type object. */
void string_dealloc(instance_object* self) {
    free(self->private);
}

/* Initialization method for the integer type object. */
void int_init(instance_object* self, vector* args) {
    if (args->items[0] == NULL) {
        THROW("int_init", "expected at least 1 value in args", 0);
        return;
    }
    self->private = args->items[0];
}

/* Initialization method for the boolean type object. */
void bool_init(instance_object* self, vector* args) {}

/* Built-in string type object. */
type_object* string;
/* Built-in integer type object. */
type_object* integer;
/* Built-in boolean type object. */
type_object* boolean;

/* Setup all the built in type objects. */
void setup_types(void) {
    string = type_new("string", map_new(), &string_init, &basic_alloc, &string_dealloc, &str_to_string, NULL);
    SETUP_TYPE(integer, "integer", int_init, int_to_string);
    SETUP_TYPE(boolean, "boolean", bool_init, int_to_string);
}

/* Free all the built in type objects. */
void finalize_types(void) {
    type_free(string);
    type_free(integer);
    type_free(boolean);
}