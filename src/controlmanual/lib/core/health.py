import os
from ..constants.base import cm_dir

__all__ = ["check_health"]

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
        "for": "iterate",
        "cat": "read",
        "type": "read"
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
    "truecolor": true,
    "log_level": "DEBUG",
    "colors": {
        "green": "#35b32a",
        "red": "#dd5a56",
        "dark_green": "#298e20"
    },
    "lowercase": false,
    "read_theme": "one-dark"
}"""

LOCK_TOML_BASE: str = """# Auto generated, do not edit manually!

[environment]
installed = false"""


def check_health() -> None:
    """Function for checking if required files and folders exist."""
    dirs = ["commands", "middleware", "logs"]
    files = {
        "config.json": CONFIG_BASE,
        "config-lock.toml": LOCK_TOML_BASE,
        os.path.join(
            "commands", "__init__.py"
        ): "# Auto generated to register this folder as a module",
    }

    for i in dirs:
        path = os.path.join(cm_dir, i)
        if not os.path.exists(path):
            print(f'directory "{i}" does not exist, creating...')
            os.makedirs(path)

    for key, value in files.items():
        path = os.path.join(cm_dir, key)
        if not os.path.exists(path):
            print(f'file "{key}" does not exist, creating...')

            with open(path, "w") as f:
                f.write(value)
