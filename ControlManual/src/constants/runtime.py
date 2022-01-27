from ..typing import Config
import json
import os
from .base import cm_dir

with open(os.path.join(cm_dir, "config.json")) as f:
    config: Config = json.loads(f.read())
