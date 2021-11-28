# -------------------------------------------------------------------------------------------
# MIT License

# Copyright (c) 2021 ZeroIntensity

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
import os
import sys
import typing
import types
import importlib
import shlex
import pathlib
import atexit
import colorama
import requests
import io
import subprocess
import click
import shutil
import platform
import zipfile
import threading
import psutil
import rich
import platform
import getpass
import datetime

from src import Client, Reload, check_health, static

VERSION: dict = {
    'string': 'Alpha 1.0.1',
    'stable': False
}

@click.command()
@click.argument('filename', default=False)
def main(filename) -> None:
    """Main file for running Control Manual."""
    check_health()
    while True:
        client = Client(VERSION)
        try:
            resp = client.start(filename)
        except KeyboardInterrupt: # supress keyboard interupt error
            return
        
        if resp == Reload:
            try:
                p = psutil.Process(os.getpid())
                for handler in p.open_files() + p.connections():
                    os.close(handler.fd)
            except:
                static.static_error('fatal error occured when reloading')

            python = sys.executable
            os.execl(python, python, *sys.argv)
        else:
            sys.exit

@atexit.register
def shutdown(): # will be called on shutdown
    print()

if __name__ == '__main__':
    main()