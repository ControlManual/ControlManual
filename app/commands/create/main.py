from genericpath import isfile
from typing import List, Dict
from src import Client # Only used for intellisense, will not work if this file is run manually.
import os

HELP: str = 'Create a file or folder.'
USAGE: str = '<type> <name> [contents] [flags]'
ARGS: dict = {'type': 'File or folder.', 'name': 'Name of the file or folder.', 'contents': 'Contents of the file.'}
FLAGS: dict = {'overwrite': 'Whether to overwrite existing files or folders.'}

def run(raw: str, args: List[str], kwargs: Dict[str, str], flags: List[str], client: Client):

    utils = client.utils

    if len(args) < 2:
        return utils.error('Please specify a type and name.')

    if args[0] == 'file':
        path: str = utils.join(client.path, args[1])
        event: str = 'created'

        if (os.path.exists(path)) and (os.path.isfile(path)):
            if 'overwrite' in flags:
                event = 'overwriten'
            else:
                return utils.error(f'"{args[1]}" already exists.')

        with open(path, 'w') as f:
            write: str = ''

            if len(args) >= 3:
                write = '"'.join(args[2:])

            f.write(write)
        
        return utils.success(f'File "{path}" was {event}.')

    if (args[0] == 'folder') or (args[0] == 'dir'):
        path: str = utils.join(client.path, args[1])
        event: str = 'created'

        if (os.path.exists(path)) and (not os.path.isfile(path)):
            if 'overwrite' in flags:
                event = 'overwriten'
            else:
                return utils.error(f'"{args[1]}" already exists.')
        
        os.makedirs(path)

        return utils.success(f'Folder "{path}" was {event}.')
    
    utils.error(f'Please specify a valid type.')