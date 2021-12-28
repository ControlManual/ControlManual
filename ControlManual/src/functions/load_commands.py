import os
import sys
import importlib
from typing import Union, Union, List, Any, Optional, AsyncGenerator
from types import ModuleType
from ..config import Config
from ..logger import log
from ..typing import Commands
from pathlib import Path

def get(command: ModuleType, target: str, default: Any = "") -> Any:
    return getattr(command, target) if hasattr(command, target) else default


async def load_commands(
    directory: str,
) -> Commands:
    """Function for creating the commands dict for the client."""
    await log("starting command loading process")
    config = Config()
    windows: bool = os.name == "nt"

    resp: dict = {}

    sys.path.append(directory)

    for i in os.listdir(directory):
        if i in ["__pycache__", ".gitignore"]:
            continue
        p = os.path.join(directory, i) # path

        if os.path.isfile(p):
            if not p.endswith('.py'):
                back = -4 if os.name == "nt" else None
                resp[i[:back]] = {"exe": os.path.join(directory, i)}

        filename = Path(p).stem
        file: str = filename if os.path.isfile(p) and p.endswith('.py') else f'{i}.main'
        command: ModuleType = importlib.import_module(
            f".commands.{file}", package="ControlManual")

        if not hasattr(command, "run"):
            continue

        cmd_help: str = get(command, "HELP")
        usage: str = get(command, "USAGE")
        package: str = get(command, "PACKAGE")
        warning: str = f"({command.WARNING})" if hasattr(
            command, "WARNING") else ""
        args: dict = get(command, "ARGS", {})
        flags: dict = get(command, "FLAGS", {})
        args_help: dict = get(command, "ARGS_HELP", {})
        live: bool = get(command, "LIVE", False)
        iterator: Optional[AsyncGenerator] = get(command, "iterator", None)

        resp[filename] = {
            "entry": command.run,
            "help": cmd_help,
            "warning": warning,
            "usage": usage,
            "args": args,
            "flags": flags,
            "package": package,
            "args_help": args_help,
            "live": live,
            "iterator": iterator
        }

    if config.raw["use_path_env"]:
        await log("reading PATH for binaries")
        for i in os.environ["PATH"].split(";" if windows else ":"):
            if os.path.exists(i):
                for x in os.listdir(i):
                    ext_raw: Union[str, List[str]] = os.environ.get(
                        "PATHEXT").split( # type: ignore
                            ",") if windows else ""

                    if (not x in resp) and (not windows):
                        resp[x] = {"exe": os.path.join(i, x)}
                    else:
                        for extension in ext_raw:
                            no_ext: str = x[:-len(extension)]
                            if not no_ext in resp:
                                resp[x] = {"exe": os.path.join(i, x)}
    return resp
