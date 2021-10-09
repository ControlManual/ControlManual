from pathlib import Path

def format_path(path: str) -> str:
    return str(Path(path).resolve())