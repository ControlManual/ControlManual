from ..typings import Config
import json
import os
from ..constants.base import cm_dir
from typeguard import typechecked
import sys

with open(os.path.join(cm_dir, "config.json")) as f:
    config: Config = json.loads(f.read())

@typechecked
def validator(config: Config):
    pass

try:
    validator(config)
except TypeError as e:
    print(e)
    sys.exit(0)