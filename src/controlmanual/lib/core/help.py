from typing import Any, Dict, List, Optional, TYPE_CHECKING

from ..typings import Commands
from ..utils import not_null
from .config import config

if TYPE_CHECKING:
    from ..client import Client

__all__ = ["HelpCommand"]

error = print


def make_str(
    commands: Commands,
    command: str,
    key: str,
    prefix: str = "",
    default: Optional[str] = None,
) -> str:
    item: str = commands[command][key]  # type: ignore

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
        if raw
        else ""
    )


def mfl(data: str) -> str:  # i kept adding a capital letter to some values
    return data[0].lower() + data[1:]


class HelpCommand:
    """Class for running help command operations."""

    def __init__(self, commands: Commands, args: List[str], client: "Client"):
        self._args = args
        self._commands = commands
        self._client = client

    @property
    def client(self):
        """Client object used."""
        return self._client

    @property
    def commands(self):
        """Raw commands dictionary."""
        return self._commands

    @property
    def args(self):
        """Command arguments."""
        return self._args

    async def print_command_help(self) -> None:
        commands = self.commands
        command = self.args[0]

        if command not in commands:
            return error("Command does not exist.")

        current = commands[command]

        if current.is_binary:
            return error("Command is an executable.")

        usage_str: str = f"[secondary]{command} [primary]"

        cmd_help: str = make_str(commands, command, "help")
        usage: str = make_str(
            commands, command, "usage", usage_str, default=usage_str + "\n"
        )
        package: str = make_str(commands, command, "package")
        args_dict: dict = current["args"]  # type: ignore
        flags_dict: dict = current["flags"]  # type: ignore
        args = flags = ""

        if (args_dict is None) or (args_dict == {}):
            args += "[danger]No arguments.[/danger]\n"
        else:
            for i, value in args_dict.items():
                args += f"[primary]{i}[/primary] - [secondary]{value}[/secondary]\n"

        if (flags_dict is None) or (flags_dict == {}):
            flags += "[danger]No flags.[/danger]\n"
        else:
            for i, value_ in flags_dict.items():
                flags += f"[primary]{i}[/primary] - [secondary]{value_}[/secondary]\n"

        self.client.print(
            f"""[primary]{cmd_help}[/primary]
    [important]Package: [secondary]{package}[/secondary]
    [important]Usage: [primary]{usage}[/primary]
    [important]Args: \n[primary]{args}[/primary]
    [important]Flags: \n[primary]{flags}[/primary]
    [important]For more information on a certain argument, use [/important][primary]"help {command} <argument>"[/primary]"""
        )

    async def print_help(self) -> None:
        commands = self.commands
        print = self.client.print

        for i in commands:
            if commands[i].is_binary:
                if config["hide_exe_from_help"]:
                    continue
                hlp = "[danger]Executable File.[/danger]"
            else:
                hlp = f'[secondary]{commands[i].help}[/secondary] [danger]{commands[i].warning}[/danger]'

            print(f"[primary]{i.lower()}[/primary] - {hlp}")
        print(
            f'[important]For more info on a command, use [/important][primary]"help <command>"[/primary]'
        )

    async def print_argument_help(self) -> None:
        command, argument = self.args

        commands = self.commands
        cmd = commands.get(command)

        if not cmd:
            return error("Command does not exist.")

        if cmd.is_binary:
            return error("Command is an executable.")

        args: Dict[str, str] = not_null(cmd.args)
        args_help: dict = not_null(cmd.args_help)

        if argument not in args:
            return error("Argument does not exist.")

        h = args_help.get(argument) or {}

        description = extract(h, "description", args[argument])
        valid_raw = extract(h, "valid_values")
        arg_type = f"\n[important]Type: [/important][secondary]{extract(h, 'type', 'String')}[/secondary]"
        valid = (
            f'\n[important]Valid Values: [/important][secondary]{"[/secondary], [secondary]".join(valid_raw)}[/secondary]'
            if valid_raw
            else ""
        )

        not_required_when: str = rq("Not_Required_When", h)
        required_when: str = rq("Required_When", h)
        when_unspecified: str = rq("When_Unspecified", h)
        ignored_when: str = rq("Ignored_When", h)

        effect_when_equals_raw: dict = extract(h, "effect_when_equals", {})
        effect_when_equals: str = (
            "\n\n[important]If this argument equals " if effect_when_equals_raw else ""
        )

        for key, value in effect_when_equals_raw.items():
            k = (
                key
                if not isinstance(key, tuple)
                else "[/secondary], [secondary]".join(key)
            )
            effect_when_equals += f"\n  - [secondary]{k} [/secondary]then [important]{mfl(value)}[/important]"

        when_flag_is_passed_raw: list = extract(h, "when_flag_is_passed", [])
        when_flag_is_passed: str = (
            "\n\nWhen the flag <x> is passed, then" if when_flag_is_passed_raw else ""
        )

        for i in when_flag_is_passed_raw:
            if len(i) < 2:
                continue

            when_flag_is_passed += f"\n  - [primary]{i[0]}[/primary][important], [/important][secondary]{mfl(i[1])}[/secondary]"

        self.client.print(
            f"[secondary]{description}[/secondary]"
            + f"{arg_type}{valid}{effect_when_equals}{when_flag_is_passed}"
            + f"{required_when}{not_required_when}{ignored_when}{when_unspecified}"
        )
