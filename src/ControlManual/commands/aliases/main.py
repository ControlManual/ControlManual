from typing import List, Dict
from ...client import Client  # Only used for intellisense, will not work if this file is run manually.

HELP: str = 'Get command aliases.'
PACKAGE: str = 'builtin'


async def run(raw: str, args: List[str], kwargs: Dict[str, str],
              flags: List[str], client: Client):

    aliases: Dict[str, str] = client.aliases
    console = client.console
    errors = client.errors

    if not aliases:
        raise errors.NothingChanged('No aliases exist.')

    for key, value in aliases.items():
        console.key_value(key, value)
