import os
from .static import JSONFile
from .cm_dir import cm_dir

config_path: str = os.path.join(cm_dir, "config.json")


class Config(JSONFile):
    """Class representing config."""
    def __init__(self) -> None:
        """Class representing config."""

        # passing in all keys for type safety
        self.truecolor = False
        self.input_sep: str = ""
        self.flag_prefix = ""
        self.colorize: bool = False
        self.check_latest = ""
        self.use_path_env: bool = False
        self.aliases: list = []
        self.comments: list = []
        self.functions: list = []
        self.help_command: str = ""
        self.seperator: str = ""
        self.errors: dict = {}
        self.columns: list = []
        self.cm_dir: str = ""
        self.hide_exe_from_help: bool = False
        self.basic: bool = False

        super().__init__(
            config_path,
            [
                "input_sep",
                "flag_prefix",
                "colorize",
                "check_latest",
                "use_path_env",
                "hide_exe_from_help",
                "aliases",
                "comments",
                "functions",
                "help_command",
                "seperator",
                "errors",
                "columns",
            ],
            {
                "errors": [
                    "unknown_command",
                    "command_error",
                    "function_open",
                    "function_not_open",
                    "function_undefined",
                    "permission_error",
                ]
            },
        )
        self.set_value("cm_dir", cm_dir)
        os.environ["cm_dir"] = cm_dir
