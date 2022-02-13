# -------------------------------------------------------------------------------------------
# MIT License

# Copyright (c) 2021 ZeroIntensity 2231puppy

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
# -------------------------------------------------------------------------------------------

from .core.health import check_health
check_health()

import logging
import os
import shutil
import sys
from pathlib import Path
from types import TracebackType
from typing import Type

import click
from rich import print as rich_print
from rich.text import Text

from . import logger
from .app import Application
from .constants import cm_dir
from .constants.info import \
    __version__  # mypy is saying version is undefined when i import it above idfk

from .utils import run

__all__ = ["main", "main_wrap"]


@click.command()
@click.option("--file", "-f", help="Run app starting with a file.", default="")
@click.option("--version", "-v", is_flag=True, help="Get the app version.")
@click.option(
    "--clean",
    "-c",
    is_flag=True,
    help="Clears all the auto generated files, and allows a clean install.",
)
def main(file: str, version: bool, clean: bool):
    check_health()
    
    if version:
        return print(f"ControlManual V{__version__}")

    if clean:
        for i in {"logs", "middleware", "commands"}:
            shutil.rmtree(os.path.join(cm_dir, i))
            print(f'Deleted "{i}"')

        for x in {"config.json", "config-lock.toml"}:
            os.remove(os.path.join(cm_dir, x))
            print(f'Deleted "{x}"')

        return

    logging.info("starting textual app")
    Application.run()


def hook(exctype: Type[BaseException], value: BaseException, tb: TracebackType):
    rich_print("[red]Internal error!")

    code = tb.tb_frame.f_code
    logging.critical(
        f"{exctype.__name__}@{code.co_filename}:{code.co_firstlineno} - {value}"
    )
    sys.__excepthook__(exctype, value, tb)

    t = Text(
        f"\nPlease check the logs for more info ({Path(logger.log_path).name}).",
        style="red",
    )
    rich_print(t)


sys.excepthook = hook  # type: ignore
# mypy has so many false positives wtf


def main_wrap():
    try:
        main()  # type: ignore
    except KeyboardInterrupt:
        sys.exit(0)


if __name__ == "__main__":
    raise Exception("must be started from __main__ due to import issues")