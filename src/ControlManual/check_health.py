import os
from .cm_dir import cm_dir
import aiofiles

config_base = """{
	"input_sep": ">>",
	"flag_prefix": "--",
	"colorize": true,
	"check_latest": true,
	"use_path_env": true,
	"hide_exe_from_help": true,
	"theme": "default",
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
		"end": "exit"
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
		"permission_error": "Control Manual does not have permission to do this."
	},
	"cm_dir": "/mnt/e/projects/python/Control Manual/app",
	"columns": ["info", "log", "directory", "exceptions"],
	"truecolor": true,
	"basic": true
}"""


async def check_health() -> None:
    """Function for checking if required files and folders exist."""
    dirs = ["commands", "middleware", "logs"]
    files = {"config.json": config_base}

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
