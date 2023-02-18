# Style Guide

**All code in Control Manual should be at most 80 characters.**

## Indentation

Indentation should be done using 4 spaces:

```c
// GOOD
void foo() {
    print("hello, world");
}

// BAD
void bar() {
  print("hello, world");
}
```

## Empty Lines

There aren't really any rules on how many newlines should seperate code, as long as you aren't doing 3+ (without a comment in between).

Empty lines should also never contain any spaces.

Generally, you're going to want to use the following as a guide:

```c
int hello() {
    if (something) {
        /* ... */
    }

    if (another_thing) {
        /* ... */
    }

    return 1;
}

int goodbye() {
    switch (thing) {
        case some_case: {
            /* ... */
        }
    }

    return 1;
}
```

## Multiple Statements

You should **never** have multiple statements seperated by a semicolon on the same line:

```c
void foo() {
    // GOOD
    bar();
    baz();
}

void foo2() {
    // BAD
    bar(); baz();
}
```

## Brackets

Brackets should stay on the same line as the statement:

```c
void x() {
    // GOOD
}

void x()
{
    // BAD
}
```

However, no code should follow the bracket:

```c
void foo() {
    {
        print("hello world"); // GOOD
    }
    { print("hello world"); } // BAD
}
```

Note that in the above code we are allowed to use bracket syntax like that, as there's no statement that it's coming from.

You should also try and omit brackets wherever possible:

```c
void foo() {
    if (condition) print("hello world"); // OK
    if (condition)
        print("hello world"); // OK
}
```

## Pointers

As you've probably seen throughout this documentation, Control Manual prefers the `type*` syntax over the more common `type *` syntax.

Whenever using a pointer inside Control Manual, always prefer this syntax. Here's an example:

```c
// GOOD
void* foo(const char* something) {
    /* ... */
}

// BAD
void *bar(const char *something) {
    /* ... */
}
```

## Keywords

Keywords should always follow the convention of `keyword value` (with the exception of `sizeof`):

```c
int foo() {
    if (something)
        return 1; // GOOD
    else return(2); // BAD
}
```

`sizeof` is the only exception to this rule:

```c
void foo() {
    something* s = safe_malloc(sizeof(something)); // GOOD
}
```

## Naming Conventions

For all function names and variables inside of Control Manual, use the `snake_case` convention:

```c
void this_is_good(int my_parameter) {
    int my_ok_variable = my_parameter + 10;
    return my_ok_variable;
}

void thisIsBad(int myParameter) {
    int myBadVariable = myParameter + 10;
    return myBadVariable;
}
```

For macros, use `SCREAMING_SNAKE_CASE`:

```c
#define THIS_IS_OK(foo)
#define ThisIsNotOk(bar)
```

## Global Variables

Global variables inside Control Manual should be prefixed with `cm_`:

```c
int cm_some_int = 0;
```

The user should also never _have_ to access the raw `cm_` variables. There should be some sort of macro that does it for you:

```c
#define SOME_INT cm_some_int
int cm_some_int = 0;

/* ... */

void do_something(void) {
    int i = SOME_INT;
}
```

## Structs

**Note:** These rules apply to all datastructures (`enum`, `union`, etc)

The **raw** struct should be named `STRUCT_TYPENAME`, and then there should be a `typedef` corresponding to it with the name of `typename`.

Some examples:

```c
typedef struct STRUCT_FOO {} foo;
typedef struct STRUCT_BAR {} bar;
typedef struct STRUCT_FOO_BAR {} foo_bar;
typedef enum ENUM_BAZ {} baz;
```

## Related Functions

Let's say you have a struct (type) named `foo`, which has two integers, `a` and `b`, and a `data*` string (`str`). Every function related to this structure must be prefixed with `foo_`.

You always need at least two functions to relate to this structure, one to create a new allocation of it (`foo_new`) and one to free it (`foo_free`). The only exceptions to this rule are `object` and `type`, as the object API works differently.

`foo_new` should always return a heap allocated version of this structure, and `foo_free` should free everything inside of it, including all `data*` references.

Example of how this would be implemented:

```c
typedef struct STRUCT_FOO {
    int a;
    int b;
    data* str;
} foo;

foo* foo_new(int a, int b, data* str) {
    foo* f = safe_malloc(sizeof(foo));
    f->a = a;
    f->b = b;
    f->str = str;
    return f;
}

void foo_free(foo* f) {
    data_free(f->str);
    free(f);
}
```

Related functions should either return that type (generally as a pointer) or take in the type as the **first** parameter.

Now, let's say you want to add `a` and `b` together. We would create a function called `add`, but we need to prefix it with `foo_` since our type is `foo`:

```c
int foo_add(foo* f) {
    return f->a + f->b;
}
```

What if we wanted to create a `foo_add_third`?

```c
int foo_add_third(foo* f, int third) {
    return f->a + f->b + third;
}
```

Notice how `f` is the first parameter, and may not be `NULL`.

---

There may also be a case where your type is stored at global scope for use at top level. For this, you should create a `type_acquire` function to safely get it from global scope. It should generally include a `NULL` check and call `FAIL` if it's `NULL`.

Here's an example:

```c
foo* foo_acquire() {
    if (!cm_foo) FAIL("foo is not ready!");
    return cm_foo;
}
```

When storing something at global scope, you typically want an initalizer function as well to be called as the runtime is starting. This should be called `typename_init`. Here's an example using our case:

```c
foo* foo_init() {
    cm_foo = foo_new(0, 0, STACK_DATA("hello world"));
}
```

## Macros

Macros should follow a few rules. If something is a simple redefinition, then it should not have parenthesis, but if it acts as a function then it should, even if it doesn't have any parameters.

For example:

```c
#define FOO cm_global_foo
// no parens should be used here

int cm_global_foo = 0;
```

```c
#define BAR() do { \
    print("hello, world") \
} while (0) \

void foo() {
    BAR();
}
```

Macros should also **never** end in a semicolon, and leave that to the user.

### Header Guards

Header guards should follow the convention of `CM_NAME_H`. The actual `NAME` doesn't really matter, as long as it's unique.

Here's an example:

```c
#ifndef CM_FOO_H
#define CM_FOO_H

#endif
```

## Reducing Indentation

**Note:** This is **not** a rule, just encouraged.

You should try and reduce indentation where you can if it makes sense to do so.

For example, take a look at the following code:

```c
int foo(int a, int b, int c) {
    if (some_val == 1) {
        /* ... */
        if (some_val == 2) {
            /* ... */
            if (some_val == 3) {
                /* ... */
                return 1;
            } else {
                return -3;
            }
        } else {
            return -2;
        }
    } else {
        return -1;
    }
}
```

This could be rewritten as:

```c
int foo(int a, int b, int c) {
    if (some_val != 1) return -1;
    if (some_val != 2) return -2;
    if (some_val != 3) return -3;

    return 1;
}
```

## Seperation

If you need to seperate something like a function call or definition, everything should line up properly and have their own indentation, like so:

```c
void long_function_signature(
    int a,
    int b,
    int c,
    int* hello,
    int** world
) {
    /* GOOD */
}

void long_function_signature(
    int a, int b,
    int c, int* hello,
    int** world
) {
    /* BAD */
}
```

```c
void foo() {
    // GOOD
    some_long_function_call(
        1,
        2,
        3,
        STACK_DATA("hello"),
        STACK_DATA("world")
    );
}

void bar() {
    // BAD
    some_long_function_call(1, 2,
                            3, STACK_DATA("hello"),
                            STACK_DATA("world")
    );
}
```

## Implicit If's

You should never check `== true`, `== false`, `== NULL`, etc.

Instead, just do it implicitly, like so:

```c
void foo() {
    int* some_pointer = NULL;
    bool something = true;

    // GOOD
    if (something) {
        /* ... */
    }

    // BAD
    if (some_pointer != NULL) {
        /* ... */
    }
}
```

## Preprocessor Directive Indentation

Indenting preprocessor directives aren't always required, but are encouraged when nesting becomes more complicated.

If you do decide to indent some directives, you should always do it by indent _before_ the `#`:

```c
// GOOD
#ifdef SOMETHING
    #ifdef ANOTHER_THING
        #define FOO
    #endif
#endif
```

```c
// BAD
#ifdef SOMETHING
#    ifdef ANOTHER_THING
#        define FOO
#    endif
#endif
```
