from pathlib import Path
import os
from typing import Final

__all__ = (
    'CONFIG_DIR',
    'CONFIG_FILE',
    'COMMANDS_DIR'
)

CONFIG_DIR: Final[str] = os.path.join(Path.home(), '.controlmanual')
CONFIG_FILE: Final[str] = os.path.join(CONFIG_DIR, 'config.json')
COMMANDS_DIR: Final[str] = os.path.join(CONFIG_DIR, 'commands')