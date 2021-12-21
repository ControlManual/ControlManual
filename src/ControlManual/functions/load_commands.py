import os
import sys
import importlib
from typing import Coroutine, Dict, Union, List, Any
from types import ModuleType
from ..config import Config
from ..logger import log


def get(command: ModuleType, target: str, default: Any = "") -> Any:
    return getattr(command, target) if hasattr(command, target) else default


async def load_commands(
    directory: str,
) -> Dict[str, Union[str, Dict[str, Union[str, Union[Coroutine, dict]]]]]:
    """Function for creating the commands dict for the client."""
    await log("starting command loading process")
    config = Config()
    windows: bool = os.name == "nt"

    resp: dict = {}

    sys.path.append(directory)
    for i in os.listdir(directory):
        if os.path.isfile(os.path.join(directory, i)):
            await log("file found, adding to executable list")
            back = -4 if os.name == "nt" else None
            resp[i[:back]] = {"exe": os.path.join(directory, i)}
        elif i != "__pycache__":
            await log("directory found, adding to executable list")
            command: ModuleType = importlib.import_module(
                f".commands.{i}.main", package="ControlManual")
            cmd_help: str = get(command, "HELP")
            usage: str = get(command, "USAGE")
            package: str = get(command, "PACKAGE")
            warning: str = f"({command.WARNING})" if hasattr(
                command, "WARNING") else ""
            args: dict = get(command, "ARGS", {})
            flags: dict = get(command, "FLAGS", {})
            args_help: dict = get(command, "ARGS_HELP", {})

            resp[i] = {
                "entry": command.run,
                "help": cmd_help,
                "warning": warning,
                "usage": usage,
                "args": args,
                "flags": flags,
                "package": package,
                "args_help": args_help,
            }

    if config.raw["use_path_env"]:
        await log("reading PATH for binaries")
        for i in os.environ["PATH"].split(";" if windows else ":"):
            if os.path.exists(i):
                for x in os.listdir(i):
                    ext_raw: Union[str, List[str]] = os.environ.get(
                        "PATHEXT").split(
                            ",") if windows else ""  # type: ignore

                    if (not x in resp) and (not windows):
                        resp[x] = {"exe": os.path.join(i, x)}
                    else:
                        for extension in ext_raw:
                            no_ext: str = x[:-len(extension)]
                            if not no_ext in resp:
                                resp[x] = {"exe": os.path.join(i, x)}

    return resp
