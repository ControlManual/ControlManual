import os
from .typings import UI
import aiofiles
from typing import Optional
from pathlib import Path
from .session import Session
from ._logging import setup_logging
import logging


async def check_health(path: Optional[str] = None):
    """Ensure all configuration files are available."""

    config_dir: str = os.path.join(path or Path.home(), ".controlmanual")
    config_file: str = os.path.join(config_dir, "config.json")
    commands_dir: str = os.path.join(config_dir, "commands")
    logs_dir: str = os.path.join(config_dir, "logs")

    if not os.path.exists(config_dir):
        logging.info("configuation directory does not exist, creating")
        os.mkdir(config_dir)
        os.mkdir(logs_dir)

        async with aiofiles.open(config_file, "w") as f:
            await f.write("{}")

        os.mkdir(commands_dir)

    return config_dir, config_file, commands_dir, logs_dir


async def load(ui: UI, cm_path: Optional[str] = None) -> Session:
    """High level function for loading a session."""
    logging.debug("loading session")
    paths = await check_health(cm_path)
    setup_logging(paths[3])
    return Session(ui, *paths)
