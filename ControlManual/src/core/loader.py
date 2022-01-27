from ..typing import Commands, CommandIterator
from types import ModuleType
from typing import Any, Optional, AsyncGenerator, Literal
from pathlib import Path
import sys
import os
from ..constants import cm_dir
import importlib
from ..utils import not_null

__all__ = ["load_commands"]

def get(command: ModuleType, target: str, default: Any = "") -> Any:
    return getattr(command, target) if hasattr(command, target) else default

async def load_directory(target: Literal["commands", "middleware"]) -> AsyncGenerator[ModuleType, None]:
    """Load modules from a directory."""
    directory = os.path.join(cm_dir, target)
    sys.path.append(directory)

    for i in os.listdir(directory):
        if i in {"__pycache__", ".gitignore", '.git'}:
            continue

        p = os.path.join(directory, i)

        filename = Path(p).stem
        file: str = filename if os.path.isfile(p) and p.endswith('.py') else f'{i}.main'
        yield importlib.import_module(f".commands.{file}", package="ControlManual")

async def load_commands() -> Commands:
    """Function for creating the commands dict for the client."""

    resp: Commands = {}

    async for command in load_directory("commands"):
        if not hasattr(command, "run"):
            continue
        
        # TODO: optimize this
        cmd_help: Optional[str] = get(command, "HELP")
        usage: Optional[str] = get(command, "USAGE")
        package: Optional[str] = get(command, "PACKAGE")
        warning: str = f"({command.WARNING})" if hasattr(command, "WARNING") else ""
        args: Optional[dict] = get(command, "ARGS", {})
        flags: Optional[dict] = get(command, "FLAGS", {})
        args_help: Optional[dict] = get(command, "ARGS_HELP", {})
        iterator: Optional[CommandIterator] = get(command, "iterator", None)

        resp[Path(not_null(command.__file__)).stem] = {
            "entry": command.run,
            "help": cmd_help,
            "warning": warning,
            "usage": usage,
            "args": args,
            "flags": flags,
            "package": package,
            "args_help": args_help,
            "iterator": iterator
        }

    return resp

# TODO: implement middleware loading