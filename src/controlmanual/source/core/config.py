import json
import os
import sys

from typeguard import typechecked

from ..constants.base import cm_dir
from ..typings import Config

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
