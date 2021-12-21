from typing import List, Dict
from ...client import Client  # Only used for intellisense, will not work if this file is run manually.

HELP: str = 'Write to a file.'
USAGE: str = '<file> <text> [flags]'
ARGS: dict = {'file': 'File to write to.', 'text': 'Text to write.'}
ARGS_HELP: dict = {}
FLAGS: dict = {'overwrite': 'Overwrite the file with the specified text.'}
PACKAGE: str = 'builtin'


async def run(raw: str, args: List[str], kwargs: Dict[str, str],
              flags: List[str], client: Client):

    utils = client.utils
    errors = client.errors

    if len(args) < 2:
        raise errors.NotEnoughArguments(
            'Please specify a file and text to write.')

    operation: str = 'a' if 'overwrite' not in flags else 'w'
    path: str = utils.get_path(client.path, args[0], file=True)

    if not path:
        raise errors.NotExists(f'File "{path}" does not exist.')

    with open(path, operation) as f:
        f.write(args[1])

    return utils.success(f'Successfully wrote to "{args[0]}"')
