import json
from dataclasses import dataclass
from .constants import CONFIG_FILE

__all__ = ('config', 'Config')

@dataclass
class Config:
    pass

with open(CONFIG_FILE) as f:
    config = Config(**json.load(f))