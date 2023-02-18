# Welcome to Control Manual's documentation!

Control Manual is a fast, customizable, and easy to use command interpreter for the modern age.

It focuses on making the life of the developer easier, giving you a cross platform native API right at your fingertips.

## What makes Control Manual different?

Control Manual throws away the old principles of Bash and Unix. In the past with Bash, every command is either built into the interpreter itself or just an executable file somewhere on your system, and is executed with arguments passed through the standard input (STDIN).

An example of this might look like this:

```c
#include <stdio.h>

int main(int argc, char** argv) {
    if (argc != 2) {
        puts("expected 1 argument");
        exit(EXIT_FAILURE);
    }

    puts(argv[1]);
    return EXIT_SUCCESS;
}
```

This program may be accessed with the following:

```
$ ./myprogram hi
hi
```

However, this can escalate quickly and become much more complicated:

-   What if you want to accept more arguments?
-   What about adding "keyword parameters" (`foo --keyword parameter`)?
-   How about a help message?

Each of which can make your code easily vulnerable to buffer overflow attacks if done incorrectly!

It can get even worse when you start having to factor in things like keeping your code cross platform.

Control Manual takes a different approach. Instead of letting the file choose how to parse it's arguments, Control Manual does it for you. This provides a simple and global way of accessing commands.

Let's take a look at the same command, reimplemented in Control Manual:

```c
#include <controlmanual.h>

COMMAND_NAME(echo)
COMMAND_DESCRIPTION("Output a message.")
COMMAND_PARAMS(
    ARG("message", "Message to output.", string)
)
COMMAND(context* c) {
    char* message;
    parse_context(c, &message);
    print(message);
    return none;
}
```

Control Manual takes care of all the argument parsing for you, as well as validating the arguments type and handling any errors in the process.

This may sound similar to PowerShell, but

## Installation

Control Manual itself cannot be used as a command interpreter. It's actually an engine for a command interpreter, that needs to be provided with a UI to interact with the user.

There are two reference implementations:

<!-- these dont exist yet -->

-   ESH (Engine Shell), may be downloaded at [esh.zintensity.dev/download](https://esh.zintensity.dev/download)
-   CMUI (Control Manual User Iterface), may be downloaded at [cmui.zintensity.dev/download](https://cmui.zintensity.dev/download)

ESH is an extremely lightweight and minimalistic implementation that will provide a similar experience to traditional interpreters, while CMUI provides a much heavier TUI with many more features.

Third party implementations can be found at [controlmanual.xyz/implementations](https://controlmanual.org/implementations)
