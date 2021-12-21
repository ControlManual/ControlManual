from typing import List, Dict
from ...client import Client  # Only used for intellisense, will not work if this file is run manually.

HELP: str = 'Displays all variables and their values.'
PACKAGE: str = 'builtin'


async def run(raw: str, args: List[str], kwargs: Dict[str, str],
              flags: List[str], client: Client):
    console = client.console

    for key, value in client.variables.items():
        console.key_value(key, value)
