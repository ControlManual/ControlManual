from .typing import Commands, ParsedString
from types import ModuleType
from typing import Any, Awaitable, Optional, AsyncGenerator, Union, List, Dict
from pathlib import Path
import sys
import os
from .constants import cm_dir
from .files import FileHandler
import importlib
import shlex

def get(command: ModuleType, target: str, default: Any = "") -> Any:
    return getattr(command, target) if hasattr(command, target) else default

class CommandWorker:
    @staticmethod
    async def load_commands() -> Commands:
        """Function for creating the commands dict for the client."""
        directory = os.path.join(cm_dir, "commands")

        config = FileHandler.get_config()
        windows: bool = os.name == "nt"

        resp: dict = {}

        sys.path.append(directory)

        for i in os.listdir(directory):
            if i in ["__pycache__", ".gitignore", '.git']:
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

        if config["use_path_env"]:
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

    @staticmethod
    async def load_middleware() -> List[Awaitable]:
        """Function for creating the commands dict for the client."""
        directory = os.path.join(cm_dir, "middleware")

        resp: List[Awaitable] = []

        sys.path.append(directory)
        for i in os.listdir(directory):
            command: ModuleType = importlib.import_module(f"middleware.{i}.main")

            resp.append(command.run)

        return resp

    @staticmethod
    async def parse(raw: str) -> ParsedString:
        """Function for parsing the input into different items."""

        try:
            split: List[str] = shlex.split(raw)
        except ValueError:
            return None

        kwargs: Dict[str, str] = {}
        flags: List[str] = []
        args: List[str] = []

        config = FileHandler.get_config()
        for i in split:
            if (("=" in i) and (len(i) >= 3) and (not i[i.index("=") - 1] == r"\\")
                    and
                (i.count("=")
                == 1)):  # a bit messy but probably the best way to do this
                split: List[str] = i.split("=")
                kwargs[split[0]] = split[1]

            elif i.startswith(config["flag_prefix"]):
                flags.append(i[2:])
            else:
                args.append(i)

        return args, kwargs, flags