from ..core.constants import CONFIG_DIR, CONFIG_FILE, COMMANDS_DIR
import os
from rich.console import Console
import aiofiles

__all__ = ('check_health',)

console = Console()

async def check_health():
    """Ensure all configuration files are available."""
    
    if not os.path.exists(CONFIG_DIR):
        console.print("Configuration directory does not exist, creating it...")
        os.mkdir(CONFIG_DIR)

        async with aiofiles.open(CONFIG_FILE, 'w') as f:
            await f.write('{}')

        os.mkdir(COMMANDS_DIR)