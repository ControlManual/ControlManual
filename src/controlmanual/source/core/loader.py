from ..typings import Commands, CommandIterator, Command
from types import ModuleType
from typing import Any, Optional, AsyncGenerator, Literal
from pathlib import Path
import sys
import os
from ..constants import cm_dir
import importlib
from ..utils import not_null
import logging


__all__ = ["load_commands"]

def get(command: ModuleType, target: str, default: Any = "") -> Any:
    return getattr(command, target) if hasattr(command, target) else default

def extract(command: ModuleType, path: str) -> Command:
    cmd_help: Optional[str] = get(command, "HELP")
    usage: Optional[str] = get(command, "USAGE")
    package: Optional[str] = get(command, "PACKAGE")
    warning: str = f"({command.WARNING})" if hasattr(command, "WARNING") else ""
    args: Optional[dict] = get(command, "ARGS", {})
    flags: Optional[dict] = get(command, "FLAGS", {})
    args_help: Optional[dict] = get(command, "ARGS_HELP", {})
    iterator: Optional[CommandIterator] = get(command, "iterator", None)


    logging.debug(str(command.run))
    return {
        "entry": command.run,
        "help": cmd_help,
        "warning": warning,
        "usage": usage,
        "args": args,
        "flags": flags,
        "package": package,
        "args_help": args_help,
        "iterator": iterator,
        "is_binary": False,
        "path": path
    }

async def load_directory(target: Literal["commands", "middleware"]) -> AsyncGenerator[Command, None]:
    """Load modules from a directory."""
    directory = os.path.join(cm_dir, target)
    sys.path.append(directory)

    for i in os.listdir(directory):
        if i in {"__pycache__", ".gitignore", '.git', '__init__.py'}:
            continue

        p = os.path.join(directory, i)
        filename: str = Path(p).stem

        if p.endswith('.py'):
            file: str = filename if os.path.isfile(p) else f'{i}.main'

            try:
                yield extract(importlib.import_module(f"controlmanual.commands.{file}"), p)
            except Exception as e:
                logging.error(f'failed to load "{file}" due to error: {e}')
        elif p.endswith('.so'):
            raise NotImplementedError("shared library commands are not yet supported")

        

async def load_commands() -> Commands:
    """Function for creating the commands dict for the client."""
    return {
        Path(not_null(command['path'])).stem: command
        async for command in load_directory("commands")
    }

# TODO: implement middleware loading