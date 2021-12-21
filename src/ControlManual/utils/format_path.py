from pathlib import Path


def format_path(path: str) -> str:
    """Function for correctly formatting a path. Warning: Does not work on Linux."""
    return str(Path(path).resolve())
