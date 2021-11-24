from typing import List, Dict
from src import Client # Only used for intellisense, will not work if this file is run manually.
import os

HELP: str = 'Reads a file.'
USAGE: str = '<file>'
ARGS: dict = {'file': 'File to read.'}
ARGS_HELP: dict = {
    'file': {
        'type': 'Path'
    }
}
PACKAGE: str = 'builtin'

def run(raw: str, args: List[str], kwargs: Dict[str, str], flags: List[str], client: Client):

    utils = client.utils

    if args == []:
        return utils.error('Please specify a file.')

    path: str = utils.join(client.path, args[0])

    if (not os.path.exists(path)) or (not os.path.isfile(path)):
        return utils.error(f'File "{args[0]}" does not exist.')

    with open(path, 'r') as f:
        utils.success(f.read())
