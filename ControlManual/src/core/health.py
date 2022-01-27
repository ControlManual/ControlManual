from ..typing import Config
from ..constants import cm_dir
import os
import aiofiles

__all__ = [
	"check_health"
]

CONFIG_BASE = """{
	"input_sep": ">>",
	"flag_prefix": "--",
	"colorize": true,
	"use_path_env": true,
	"hide_exe_from_help": true,
	"aliases": {
		"echo": "print",
		"ls": "listdir",
		"md": "create dir",
		"mk": "create file",
		"set": "var",
		"cd": "path",
		"rd": "remove dir",
		"rf": "remove file",
		"delete": "remove",
		"cls": "clear",
		"touch": "create file",
		"config": "json '{config}'",
		"end": "exit",
		"for": "iterate"
	},
	"comments": ["//", "#"],
	"functions": ["{", "}"],
	"help_command": "help",
	"seperator": ";",
	"errors": {
		"unknown_command": "Unknown command. Use help to see commands.",
		"command_error": "Exception occured when running command {cmd}.",
		"function_open": "Function is already open.",
		"function_not_open": "Function is not open.",
		"function_undefined": "No function currently defined.",
		"permission_error": "Permission denied."
	},
	"columns": ["info", "log", "directory", "exceptions"],
	"truecolor": true,
	"basic": true,
	"basic_input": false
}"""

LOCK_TOML_BASE: str = """# Auto generated, do not edit manually!

[environment]
installed = false"""

async def check_health() -> None:
    """Function for checking if required files and folders exist."""
    dirs = ["commands", "middleware", "logs"]
    files = {"config.json": CONFIG_BASE, "config-lock.toml": LOCK_TOML_BASE}

    for i in dirs:
        path = os.path.join(cm_dir, i)
        if not os.path.exists(path):
            print(f'directory "{i}" does not exist, creating...')
            os.makedirs(path)

    for key, value in files.items():
        path = os.path.join(cm_dir, key)
        if not os.path.exists(path):
            print(f'file "{key}" does not exist, creating...')

            async with aiofiles.open(path, "w") as f:
                await f.write(value)
