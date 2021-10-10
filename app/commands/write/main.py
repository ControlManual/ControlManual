from typing import List, Dict
from src import Client # Only used for intellisense, will not work if this file is run manually.
import os

HELP: str = 'Write to a file.'
USAGE: str = '<file> <text> [flags]'
ARGS: dict = {'file': 'File to write to.', 'text': 'Text to write.'}
FLAGS: dict = {'overwrite': 'Overwrite the file with the specified text.'}
PACKAGE: str = 'builtin'

def run(raw: str, args: List[str], kwargs: Dict[str, str], flags: List[str], client: Client):

    utils = client.utils

    if len(args) < 2:
        return utils.error('Please specify a file and text to write.')

    operation: str = 'a' if 'overwrite' not in flags else 'w'
    path: str = utils.get_path(client.path, args[0], file = True)

    if not path:
        return utils.error(f'File "{path}" does not exist.')

    with open(path, operation) as f:
        f.write(args[1])
    
    return utils.success(f'Successfully wrote to "{args[0]}"')