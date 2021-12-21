from typing import List, Dict
from ...client import Client  # Only used for intellisense, will not work if this file is run manually.
import os

HELP: str = 'List the current directory.'
USAGE: str = '<directory>'
ARGS: dict = {'directory': "Directory to search."}
ARGS_HELP: dict = {
    'directory': {
        'type': 'Path',
        'when_unspecified': 'Uses the current directory.'
    }
}
PACKAGE: str = 'builtin'


async def run(raw: str, args: List[str], kwargs: Dict[str, str],
              flags: List[str], client: Client):
    utils = client.utils
    errors = client.errors
    console = client.console

    path: str = utils.get_path(client.path, '' if len(args) == 0 else args[0])

    if not path:
        raise errors.NotExists(f'Folder "{args[0]}" does not exist.')

    final: str = ''
    for i in os.listdir(path):
        final += f'[primary]{i}[/primary] ' if os.path.isfile(
            os.path.join(path, i)) else f'[secondary]{i}[/secondary] '

    console.print(final)
