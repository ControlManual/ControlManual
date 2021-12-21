from ..utils import error
from ..console import console


def make_str(commands: dict,
             command: str,
             key: str,
             prefix: str = "",
             default=None) -> str:
    item = commands[command][key]

    if item:
        return prefix + item + "\n"
    else:
        return default or f"[danger]No {key}.\n"


async def print_command_help(commands: dict, command: str) -> None:
    if not (command in commands):
        return error(f"Command does not exist.")

    if "exe" in commands[command]:
        return console.error("Command is an executable.")

    usage_str: str = f"[secondary]{command} [primary]"

    cmd_help: str = make_str(commands, command, "help")
    usage: str = make_str(commands,
                          command,
                          "usage",
                          usage_str,
                          default=usage_str + "\n")
    package: str = make_str(commands, command, "package")
    args_dict: dict = commands[command]["args"]
    flags_dict: dict = commands[command]["flags"]
    args = flags = ""

    if (args_dict is None) or (args_dict == {}):  # TODO: optimize
        args += f"[danger]No arguments.\n"
    else:
        if args_dict == {}:
            args += make_str(commands, command, "args")
        else:
            for i in args_dict:
                args += f"[primary]{i}[/primary] - [secondary]{args_dict[i]}[/secondary]\n"

    if (flags_dict is None) or (flags_dict == {}):
        flags += f"[danger]No flags.\n"
    else:
        if flags_dict == {}:
            flags += make_str(commands, command, "flags")
        else:
            for i in flags_dict:
                flags += f"[primary]{i}[/primary] - [secondary]{flags_dict[i]}[/secondary]\n"

    console.print(f"""[primary]{cmd_help}[/primary]
[important]Package: [secondary]{package}[/secondary]
[important]Usage: [primary]{usage}[/primary]
[important]Args: \n[primary]{args}[/primary]
[important]Flags: \n[primary]{flags}[/primary]
[important]For more information on a certain argument, use [primary]"help {command} <argument>"
""")
