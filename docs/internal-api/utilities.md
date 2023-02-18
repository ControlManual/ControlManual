# Useful Utilities

## Allocation

There are 3 utilities for helping you with allocation, `safe_malloc`, `safe_calloc`, and `safe_realloc`.

All they do is run a `NULL` check after the actual allocation call, and crash Control Manual if it returns `NULL`.

Typically, you would use the following:

```c
void foo() {
    char* bar = malloc(3);
    if (!bar) /* ... */
    strcpy(bar, "hi");
}
```

Instead, we can use `safe_malloc` to avoid the `NULL` check:

```c
void foo() {
    char* bar = safe_malloc(3);
    strcpy(bar, "hi");
}
```

## Platforms

Control Manual only supports POSIX compliant platforms and Windows. You can check the current system through the `PLATFORM_POSIX` and `PLATFORM_WIN` macros:

```c
char* find_platform() {
#ifdef PLATFORM_POSIX
    return "posix";
#else
    return "windows";
#endif
}
```

## Crashing

We talked about above how `safe_` functions crash Control Manual, but how do they actually do it?

Control Manual should _try_ and shut down through `exit()` so things like TContext finalizers get called, but if something like a memory allocation fails then that means something horribly wrong has happened and we need to shutdown with debug information. To do this, we should use the `FAIL` macro:

```c
void something() {
    void* some_result = some_os_api();
    if (!some_result) FAIL("some_os_api failed");
}
```

## Visibility

There are four macros for changing the visibility of a function when it's compiled to a library.

The first is `EXPORT`, which exports a symbol to the DLL:

```c
EXPORT void foo() {
    /* ... */
}
```

Next, `IMPORT` which does the same thing as `EXPORT`, except on Windows:

```c
IMPORT void foo() {
    /* ... */
}
```

`LOCAL`, which makes something private:

```c
LOCAL void foo() {
    /* ... */
}
```

And finally, the most useful of them, `API`:

```c
API void foo() {
    /* ... */
}
```

`API` will export to the DLL, and then import from it when including it on Windows. All Control Manual public API functions should use this macro.

## Function Pointers

Control Manual has a utility for making function pointer definitions simpler, called `FUNCTYPE`.

It should be used like the following:

```c
typedef FUNCTYPE(name, return_value, (/* params */));
```

As an example, here's how to create a `typedef` for the function `void* foo(int a, char* b)`:

```c
typedef FUNCTYPE(my_callback, void*, (int, char*));

void something(my_callback f) {
    /* ... */
}

void* foo(int a, char* b) {
    /* ... */
}

void bar() {
    something(foo);
}
```
