from typing import List, Dict
from src import Client # Only used for intellisense, will not work if this file is run manually.

HELP: str = 'Displays all variables and their values.'
USAGE: str = ''
ARGS: dict = {}
FLAGS: dict = {}
PACKAGE: str = 'builtin'

def run(raw: str, args: List[str], kwargs: Dict[str, str], flags: List[str], client: Client):
    theme = client.theme

    for key, value in client.variables.items():
        print(f'{theme.secondary}{key}{theme.reset} - {theme.primary}{value}{theme.reset}')