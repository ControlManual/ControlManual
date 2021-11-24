from typing import List, Dict
from src import Client # Only used for intellisense, will not work if this file is run manually.

HELP: str = 'Get command aliases.'
PACKAGE: str = 'builtin'

def run(raw: str, args: List[str], kwargs: Dict[str, str], flags: List[str], client: Client):

    aliases: Dict[str, str] = client.aliases
    utils = client.utils
    theme = client.theme

    if not aliases:
        return utils.error('No aliases exist.')

    for i in aliases:
        text: str = f'{theme.primary}{i} {utils.reset}- {theme.secondary}{aliases[i]}{utils.reset}'
        print(text)