from typing import List, Dict
from src import Client # Only used for intellisense, will not work if this file is run manually.
import os
import shutil

HELP: str = 'Removes a file or directory.'
USAGE: str = '<type> <name>'
ARGS: dict = {'type': 'Type of file to remove.', 'name': 'Name of the file to remove.'}
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
    }
}
PACKAGE: str = 'builtin'

def run(raw: str, args: List[str], kwargs: Dict[str, str], flags: List[str], client: Client):

    utils = client.utils

    if len(args) < 2:
        return utils.error('Please specify a type and name.')

    if args[0] == 'file':
        path: str = utils.get_path(client.path, args[1], file = True)

        if not path:
            return utils.error(f'File "{args[1]}" does not exist.')
        
        os.remove(path)

        return utils.success(f'Successfully removed file "{args[1]}".')
    
    if (args[0] == 'dir') or (args[0] == 'folder'):
        path: str = utils.get_path(client.path, args[1])

        if not path:
            return utils.error(f'Folder "{args[1]}" does not exist.')
        
        shutil.rmtree(path)

        return utils.success(f'Successfully removed folder "{args[1]}".')
    
    return utils.error('Please specify a valid type.')
    