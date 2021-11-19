from ..theme import *
from .colors import reset

def success(message: str) -> None:
    print(f'{primary}{message}{reset}')

def make_success(message: str) -> str:
    return f'{primary}{message}{reset}'