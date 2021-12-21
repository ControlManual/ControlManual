from genericpath import isfile
from typing import List, Dict
from ...client import Client  # Only used for intellisense, will not work if this file is run manually.
import os

HELP: str = 'Create a file or folder.'
USAGE: str = '<type> <name> [contents] [flags]'
ARGS: dict = {
    'type': 'Type of file to create.',
    'name': 'Name of the file.',
    'contents': 'Contents of the file.'
}
ARGS_HELP: dict = {
    'type': {
        'valid_values': ['file', 'dir', 'folder'],
        'effect_when_equals': {
            'file': 'Argument "name" will look for a file.',
            ('folder', 'dir'): 'Argument "name" will look for a folder.'
        }
    },
    'name': {
        'type': 'Path'
    },
    'contents': {
        'ignored_when': 'Argument "type" is "folder" or "dir".'
    }
}
FLAGS: dict = {'overwrite': 'Whether to overwrite existing files or folders.'}
PACKAGE: str = 'builtin'


async def run(raw: str, args: List[str], kwargs: Dict[str, str],
              flags: List[str], client: Client):

    utils = client.utils
    errors = client.errors

    if len(args) < 2:
        raise errors.NotEnoughArguments('Please specify a type and name.')

    if args[0] == 'file':
        path: str = utils.join(str(client.path), args[1])
        event: str = 'created'

        if (os.path.exists(path)) and (os.path.isfile(path)):
            if 'overwrite' in flags:
                event = 'overwriten'
            else:
                raise errors.Exists(f'"{args[1]}" already exists.')

        with open(path, 'w') as f:
            write: str = ''

            if len(args) >= 3:
                write = '"'.join(args[2:])

            f.write(write)

        return utils.success(f'File "{path}" was {event}.')

    if (args[0] == 'folder') or (args[0] == 'dir'):
        path: str = utils.join(str(client.path), args[1])
        event: str = 'created'

        if (os.path.exists(path)) and (not os.path.isfile(path)):
            if 'overwrite' in flags:
                event = 'overwriten'
            else:
                raise errors.Exists(f'"{args[1]}" already exists.')

        os.makedirs(path)

        return utils.success(f'Folder "{path}" was {event}.')

    raise errors.InvalidArgument(f'Please specify a valid type.')
