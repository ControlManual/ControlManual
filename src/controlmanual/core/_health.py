import os
from rich.console import Console
import aiofiles
from pathlib import Path

__all__ = ('check_health',)

__all__ = (
    'CONFIG_DIR',
    'CONFIG_FILE',
    'COMMANDS_DIR',
    'LOGS_DIR'
)

CONFIG_DIR: str = os.path.join(Path.home(), '.controlmanual')
CONFIG_FILE: str = os.path.join(CONFIG_DIR, 'config.json')
COMMANDS_DIR: str = os.path.join(CONFIG_DIR, 'commands')
LOGS_DIR: str = os.path.join(CONFIG_DIR, 'logs')

console = Console()

async def check_health():
    """Ensure all configuration files are available."""
    
    if not os.path.exists(CONFIG_DIR):
        console.print("Configuration directory does not exist, creating it...")
        os.mkdir(CONFIG_DIR)
        os.mkdir(LOGS_DIR)

        async with aiofiles.open(CONFIG_FILE, 'w') as f:
            await f.write('{}')

        os.mkdir(COMMANDS_DIR)