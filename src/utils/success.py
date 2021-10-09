from .colors import *

def success(message: str) -> None:
    print(f'{bright_green}{message}{reset}')

def make_success(message: str) -> str:
    return f'{bright_green}{message}{reset}'