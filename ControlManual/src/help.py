from .typing import Commands
from typing import Any
from .files import get_config

__all__ = ["HelpCommand"]

error = print

def make_str(commands: dict, command: str, key: str, prefix: str = "", default = None) -> str:
    item = commands[command][key]

    if item:
        return prefix + item + "\n"
    else:
        return default or f"[danger]No {key}.[/danger]\n"

def extract(col: dict, key: str, default: Any = "") -> Any:
    return col.get(key) or default


def rq(key: str, col: dict) -> str:
    raw: str = extract(col, key.lower())
    return (
        f'\n[important]{key.replace("_", " ")}:[/important] [secondary]{raw}[/secondary]'
        if raw else "")


def mfl(data: str) -> str:  # i kept adding a capital letter to some values
    return data[0].lower() + data[1:]

class HelpCommand:
    def __init__(self, commands: Commands, console):
        self._console = console
        self._commands = commands

    @property
    def commands(self):
        """Raw commands dictionary."""
        return self._commands

    @property
    def console(self):
        """Raw console object."""
        return self._console

    async def print_command_help(self, command: str) -> None:
        commands = self.commands
        console = self.console

        if not (command in commands):
            return error(f"Command does not exist.")

        current = commands[command]

        if "exe" in current:
            return await console.error("Command is an executable.")

        usage_str: str = f"[secondary]{command} [primary]"

        cmd_help: str = make_str(commands, command, "help")
        usage: str = make_str(commands,
                            command,
                            "usage",
                            usage_str,
                            default=usage_str + "\n")
        package: str = make_str(commands, command, "package")
        args_dict: dict = current["args"]
        flags_dict: dict = current["flags"]
        args = flags = ""

        if (args_dict is None) or (args_dict == {}):  # TODO: optimize
            args += f"[danger]No arguments.[/danger]\n"
        else:
            if args_dict == {}:
                args += make_str(commands, command, "args")
            else:
                for i in args_dict:
                    args += f"[primary]{i}[/primary] - [secondary]{args_dict[i]}[/secondary]\n"

        if (flags_dict is None) or (flags_dict == {}):
            flags += f"[danger]No flags.[/danger]\n"
        else:
            if flags_dict == {}:
                flags += make_str(commands, command, "flags")
            else:
                for i in flags_dict:
                    flags += f"[primary]{i}[/primary] - [secondary]{flags_dict[i]}[/secondary]\n"

        await console.print(f"""[primary]{cmd_help}[/primary]
    [important]Package: [secondary]{package}[/secondary]
    [important]Usage: [primary]{usage}[/primary]
    [important]Args: \n[primary]{args}[/primary]
    [important]Flags: \n[primary]{flags}[/primary]
    [important]For more information on a certain argument, use [/important][primary]"help {command} <argument>"[/primary]""")

async def print_help(self) -> None:
    commands = self.commands
    console = self.console

    config = get_config()

    for i in commands:
        if "exe" in commands[i]:
            if config["hide_exe_from_help"]:
                continue
            hlp = f"[danger]Executable File.[/danger]"
        else:
            hlp = f'[secondary]{commands[i]["help"]}[/secondary] [danger]{commands[i]["warning"]}[/danger]'

        await console.print(f"[primary]{i.lower()}[/primary] - {hlp}")
    await console.print(
        f'[important]For more info on a command, use [/important][primary]"help <command>"[/primary]'
    )

async def print_argument_help(self, command: str, argument: str) -> None:
    commands = self.commands


    cmd = commands.get(command)

    if not cmd:
        return error("Command does not exist.")

    if "exe" in cmd:
        return error("Command is an executable.")

    args = cmd["args"]
    args_help = cmd["args_help"]

    if argument not in args:
        return error("Argument does not exist.")

    h = args_help.get(argument) or {}

    description = extract(h, "description", args[argument])
    valid_raw = extract(h, "valid_values")
    arg_type = f"\n[important]Type: [/important][secondary]{extract(h, 'type', 'String')}[/secondary]"
    valid = (
        f'\n[important]Valid Values: [/important][secondary]{f"[/secondary], [secondary]".join(valid_raw)}[/secondary]'
        if valid_raw else "")

    not_required_when: str = rq("Not_Required_When", h)
    required_when: str = rq("Required_When", h)
    when_unspecified: str = rq("When_Unspecified", h)
    ignored_when: str = rq("Ignored_When", h)

    effect_when_equals_raw: dict = extract(h, "effect_when_equals", {})
    effect_when_equals: str = (f"\n\n[important]If this argument equals "
                               if effect_when_equals_raw else "")

    for key, value in effect_when_equals_raw.items():
        k = key if not isinstance(
            key, tuple) else f"[/secondary], [secondary]".join(key)
        effect_when_equals += (
            f"\n  - [secondary]{k} [/secondary]then [important]{mfl(value)}[/important]"
        )

    when_flag_is_passed_raw: list = extract(h, "when_flag_is_passed", [])
    when_flag_is_passed: str = ("\n\nWhen the flag <x> is passed, then"
                                if when_flag_is_passed_raw else "")

    for i in when_flag_is_passed_raw:
        if len(i) < 2:
            continue

        when_flag_is_passed += f"\n  - [primary]{i[0]}[/primary][important], [/important][secondary]{mfl(i[1])}[/secondary]"

    print(
        f"[secondary]{description}[/secondary]{arg_type}{valid}{effect_when_equals}{when_flag_is_passed}{required_when}{not_required_when}{ignored_when}{when_unspecified}"
    )