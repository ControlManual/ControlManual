from typing import List, Dict
from src import Client # Only used for intellisense, will not work if this file is run manually.

HELP: str = 'Append to the current directory.'
USAGE: str = '<directory>'
ARGS: dict = {'directory': 'Directory to append to.'}
ARGS_HELP: dict = {
    'directory': {
        'type': 'Path'
    }
}
PACKAGE: str = 'builtin'

def run(raw: str, args: List[str], kwargs: Dict[str, str], flags: List[str], client: Client):
    utils = client.utils

    if not args:
        return utils.error('Please specify a directory.')

    path: str = utils.get_path(client.path, raw)

    if not path:
        return utils.error(f'Directory "{raw}" does not exist.')

    if path == client.path_str:
        return utils.error(f'Nothing changed, path is already {path}.')

    client.change_path(
        utils.format_path(path)
    )

    utils.success('Successfully updated directory.')
