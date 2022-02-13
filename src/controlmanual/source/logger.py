import logging
import os
from datetime import datetime

from .constants import cm_dir
from .core.config import config

__all__ = ["log_path"]

now = datetime.now()
log_path: str = os.path.join(cm_dir, "logs", now.strftime("%m.%d.%Y_%H-%M-%S.log"))

logging.basicConfig(
    filename=log_path,
    format="[%(asctime)s] %(levelname)s: %(message)s",
    filemode="w",
    level=config["log_level"],
    datefmt="%H:%M:%S",
)
