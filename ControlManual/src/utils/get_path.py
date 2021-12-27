import os
from typing import Union, Optional
from pathlib import Path

PathLike = Union[Path, str] # probably a builtin thing for this but idc

def validate(path: PathLike) -> Optional[str]:
    if (os.path.isfile(path)) or (os.path.isdir(path)):
        return str(path)
    return None

def get_path(
    current_path: PathLike, path: PathLike, file: bool = False
) -> Optional[str]:
    """Function for checking if a path exists globally, or in the current directory. Returns None if not found."""
    merged: str = os.path.join(current_path, path)
    if not os.path.exists(merged):
        if not os.path.exists(path):
            return None
        else:
            if os.path.isfile(path):
                return validate(path) if file else None
            else:
                return validate(path) if not file else None
    else:
        if os.path.isfile(merged):
            return validate(merged) if file else None
        else:
            return validate(merged) if not file else None
