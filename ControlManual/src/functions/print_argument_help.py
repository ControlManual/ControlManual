from ..utils import error
from ..console import console
from typing import Any
from ..typing import Commands


def extract(col: dict, key: str, default: Any = "") -> Any:
    return col.get(key) or default


def rq(key: str, col: dict) -> str:
    raw: str = extract(col, key.lower())
    return (
        f'\n[important]{key.replace("_", " ")}:[/important] [secondary]{raw}[/secondary]'
        if raw else "")


def mfl(data: str) -> str:  # i kept adding a capital letter to some values
    return data[0].lower() + data[1:]


async def print_argument_help(commands: Commands, command: str,
                              argument: str) -> None:
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

    console.print(
        f"[secondary]{description}[/secondary]{arg_type}{valid}{effect_when_equals}{when_flag_is_passed}{required_when}{not_required_when}{ignored_when}{when_unspecified}"
    )
