import os


def join(path_1: str, *args) -> str:
    """Function for joining paths."""
    return os.path.join(path_1, *args)
