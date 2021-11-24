from typing import List, Dict
from src import Client # Only used for intellisense, will not work if this file is run manually.
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

def run(raw: str, args: List[str], kwargs: Dict[str, str], flags: List[str], client: Client):
    utils = client.utils
    path: str = utils.get_path(client.path, '' if len(args) == 0 else args[0])
    
    if not path:
        return utils.error(f'Folder "{args[0]}" does not exist.')

    client.utils.success(
        "\n".join(
            os.listdir(
                path
            )
        )
    )