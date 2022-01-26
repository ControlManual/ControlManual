import logging
from .constants import cm_dir
import os
from datetime import datetime
from .files import get_config

__all__ = []

now = datetime.now()
log_path: str = os.path.join(cm_dir, "logs", now.strftime("%m.%d.%Y_%H-%M-%S.log"))
config = get_config()

logging.basicConfig(filename = log_path, format = '[%(asctime)s] %(name)s:%(levelname)s - %(message)s', filemode = 'w', level = config["log_level"], datefmt = '%H:%M:%S')