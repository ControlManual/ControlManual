# Top Context's

## What is a Top Context (TContext)?

A top context (`tcontext`) is a struct that holds information about the current state of the Control Manual runtime.

It holds the following items (each of which may be `NULL`, except `state`):

-   The current "name" (the function name, command name, plugin name, etc)
-   The current origin (where the `tcontext` was advanced)
-   The last `tcontext` frame
-   The current `context` of a command
-   The state of the runtime

## States

As of now, there are 6 states:

```c
typedef enum ENUM_TCONTEXT_STATE {
    INITALIZING, /* runtime is still initalizing */
    COMMAND_LOAD, /* commands are being loaded */
    MIDDLEWARE_LOAD, /* middleware is being loaded */
    PLUGIN_LOAD, /* plugins are being loaded */
    COMMAND_LOOP, /* runtime is ready, we are now in the command loop */
    MIDDLEWARE_EXEC, /* we are executing middleware */
    COMMAND_EXEC, /* we are executing a command */
    OBJECT_EXEC, /* we are executing something on an object */
    FINALIZING, /* runtime is shutting down */
} tcontext_state;
```

-   `INITIALIZING` is the state right after `tcontext_init` was called.
-   `COMMAND_LOAD` is the state during the call of `load_commands`.
-   `MIDDLEWARE_LOAD` is the state during the call of `load_middleware`.
-   `PLUGIN_LOAD` is the state during the call of `load_plugins`.
-   `COMMAND_LOOP` can be the state during any stage of the command interpreting process.
-   `MIDDLEWARE_EXEC` is the state during execution of middleware.
-   `COMMAND_EXEC` is the state as a command is getting executed.
-   `OBJECT_EXEC` is the state when an attribute on a Control Manual object (or the object itself) is being called.
-   `FINALIZING` is the state when the interpreter is shutting down and in the deallocation process.

## Advancing the TContext

The TContext works in a stack like way. To push a new context, you generally want to use the `ADVANCE_DEFAULT` macro:

```c
ADVANCE_DEFAULT(
    STACK_DATA("some name"), /* or NULL */
    foo, // this is the name of the current (C) function
    COMMAND_LOOP // state enum
);
```

Sometimes you might have a `context` to pass with the TContext. For this scenario, you may use `ADVANCE_DEFAULT_CTX`, which works almost the same way:

```c
ADVANCE_DEFAULT_CTX(
    NULL, // name
    foo, // origin
    my_context, // context*
    COMMAND_LOOP // state
);
```

Once you are at the end of your TContext (generally at the end of the current function), you need to remove it through `tcontext_pop`:

```c
ADVANCE_DEFAULT(
    STACK_DATA("some name"), /* or NULL */
    foo, // this is the name of the current (C) function
    COMMAND_LOOP // state enum
);
some_method();
tcontext_pop();
```

## Acquiring the TContext

This is super simple. Either use `tcontext_acquire`, or the `TC` macro. Both do the same:

```c
tcontext* tc = TC();
```

Note that both of these ensure that the TContext has been initalized first, and will crash Control Manual if it's not set up.

If you don't want to check for initialized and take the raw TContext variable (which may be `NULL` depending on where you called it), you may directly access `cm_runtime_tcontext`:

```c
if (cm_runtime_tcontext) {
    // ...
} else {
    // forget about it
}
```

## Setting a Finalizer

If you would like code to be run when the TContext is cleaned up, you may use `tcontext_add_finalizer` or `FINALIZE`:

```c
void my_cleanup(tcontext* tc) {
    print("mom i'm cleaning my room");
}

void foo() {
    /* assuming the tcontext is ready */
    FINALIZE(my_cleanup);
}
```

If you had already acquired the TContext from somewhere earlier, you should use `tcontext_add_finalizer` to avoid a double lookup:

```c
void my_cleanup(tcontext* tc) {
    print("mom i'm cleaning my room");
}

void foo() {
    tcontext* tc = TC();
    /* ... */
    tcontext_add_finalizer(tc, my_cleanup);
}
```
