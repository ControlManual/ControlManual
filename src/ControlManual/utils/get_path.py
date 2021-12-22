import os
from typing import Union, Optional
from pathlib import Path


def get_path(
    current_path: str, path: Union[str, Path], file: bool = False
) -> Optional[str]:
    """Function for checking if a path exists globally, or in the current directory. Returns None if not found."""
    merged: str = os.path.join(current_path, path)
    if not os.path.exists(merged):
        if not os.path.exists(path):
            return None
        else:
            if os.path.isfile(path):
                return str(path) if file else None
            else:
                return str(path) if not file else None
    else:
        if os.path.isfile(merged):
            return merged if file else None
        else:
            return merged if not file else None
