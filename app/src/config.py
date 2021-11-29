import os
from .static import JSONFile
from .check_health import cm_dir, check_health

config_path: str = os.path.join(cm_dir, 'config.json')

check_health()

class Config(JSONFile):
    """Class representing config."""
    def __init__(self) -> None:
        """Class representing config."""
        super().__init__(config_path, [
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
            "theme",
            "errors",
            "columns"
        ], {
                "errors": [
                    "unknown_command",
                    "command_error",
                    "function_open",
                    "function_not_open",
                    "function_undefined",
                    "permission_error"
                ]
            }
        )
        self.set_value('cm_dir', cm_dir)