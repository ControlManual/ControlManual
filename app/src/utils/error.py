from .colors import *

def error(message: str) -> None:
    print(f'{bright_red}{message}{reset}')

def make_error(message: str) -> str:
    return f'{bright_red}{message}{reset}'