from ..theme import *
from .colors import reset

def error(message: str) -> None:
    print(f'{danger}{message}{reset}')

def make_error(message: str) -> str:
    return f'{danger}{message}{reset}'