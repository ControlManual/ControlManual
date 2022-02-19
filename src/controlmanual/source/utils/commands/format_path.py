from pathlib import Path
from typing import Union

def format_path(path: Union[str, Path]):
    return Path(path).resolve()