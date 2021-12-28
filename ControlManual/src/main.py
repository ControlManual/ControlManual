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

# Dependencies
from rich.console import Console

tmp = Console()

import os
import sys
import typing
import types
import importlib
import shlex
import pathlib
import atexit
import colorama
import io
import subprocess
import click
import shutil
import platform
import rethread
import psutil
import rich
import platform
import getpass
import datetime
import distro
import time
import watchdog
import tempfile
import inspect
import asyncio
import aiofiles
import aiohttp
import py7zr
import toml
import contextlib
import github

from . import static, info
from .check_health import check_health, cm_dir

@atexit.register
def shutdown():
    from .logger import flush
    print()
    asyncio.run(flush())

async def main(filename: str) -> None:
    """Main file for running Control Manual."""
    from .logger import log, flush
    from .client import Client, Reload

    if float(platform.python_version()[:3]) < 3.8:
        static.static_error("invalid python version - at least 3.8 is required")

    while True:

        client = await Client(info.__version__)
        await log("entering main loop")
        try:
            resp = await client.start(filename)
        except Exception as e:
            client._thread_running = False
            await log(f"exception occured: {e}")
            raise e

        if resp == Reload:
            await log("reload invoked, starting process")
            await flush()
            try:
                p = psutil.Process(os.getpid())
                for handler in p.open_files() + p.connections(
                ):  # type: ignore
                    os.close(handler.fd)
            except:
                static.static_error("fatal error occured when reloading")

            python = sys.executable
            await log("restarting app")
            os.execl(python, python, *sys.argv)
        else:
            sys.exit


@click.command()
@click.option("--file", "-f", help="Run app starting with a file.", default = '')
@click.option("--version", "-v", is_flag=True, help="Get the app version.")
@click.option("--clean", "-c", is_flag=True, help="Clears all the auto generated files, and allows a clean install (with the exception of source code).")
def main_sync(file: str, version: bool, clean: bool):
    asyncio.run(check_health())

    if version:
        return print(f'ControlManual V{info.__version__}')

    if clean:
        for i in ['logs', 'middleware', 'commands']:
            shutil.rmtree(os.path.join(cm_dir, i))
        
        for x in ['config.json', 'config-lock.toml']:
            os.remove(os.path.join(cm_dir, x))

    asyncio.run(main(file))

def main_wrap():
    try:
        main_sync() # type: ignore
    except KeyboardInterrupt:
        sys.exit(0)


if __name__ == "__main__":
    main_wrap()
