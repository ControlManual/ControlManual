import logging
from .constants import cm_dir, config
import os
from datetime import datetime

__all__ = []

now = datetime.now()
log_path: str = os.path.join(cm_dir, "logs", now.strftime("%m.%d.%Y_%H-%M-%S.log"))

logging.basicConfig(filename = log_path, format = '[%(asctime)s] %(name)s:%(levelname)s - %(message)s', filemode = 'w', level = config["log_level"], datefmt = '%H:%M:%S')