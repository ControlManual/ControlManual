import os

BASE: str = """from typing import List, Dict
from ...client import Client # Only used for intellisense, will not work if this file is run manually.

HELP: str = 'Basic command.'
USAGE: str = '<message>'
ARGS: dict = {'message': 'Message to print.'}
ARGS_HELP: dict = {
    'message': {
        'not_required_when': 'Flag "hello_world" is passed.'
    }
}
FLAGS: dict = {'hello_world': 'Whether to print "Hello World".'}
PACKAGE: str = '{pkg_name}'

async def run(raw: str, args: List[str], kwargs: Dict[str, str], flags: List[str], client: Client):
    config = client.config # Config file operations
    utils = client.utils # All utilities
    api = client.api # All functions regarding the API
    static = client.static # Functions with no dependencies on the rest of the project
    errors = client.errors # All error classes

    if not args:
        raise errors.NotEnoughArguments('Please specify a message.')

    if 'hello_world' in flags:
        print("Hello World")
    else:
        print(args[0])
"""

MIDDLEWARE: str = '''from typing import List, Dict
from ...client import Client # Only used for intellisense, will not work if this file is run manually.

async def run(command: str, raw: str, args: List[str], kwargs: Dict[str, str], flags: List[str], client: Client):
    print(f'Command "{command}" was run!')
'''

from typing import List, Dict
from ...client import Client  # Only used for intellisense, will not work if this file is run manually.

HELP: str = 'Initalizes a command or middleware.'
USAGE: str = '<name> [flags]'
ARGS: dict = {'name': 'Name of the command or middleware.'}
FLAGS: dict = {
    'here': 'Initalizes the command in the current directory.',
    'middleware': 'Initalizes middleware instead of a command.'
}
PACKAGE: str = 'builtin'


async def run(raw: str, args: List[str], kwargs: Dict[str, str],
              flags: List[str], client: Client):
    utils = client.utils
    config = client.config
    errors = client.errors

    typ: str = 'command' if 'middleware' not in flags else 'middleware'

    if not args:
        raise errors.NotEnoughArguments(f'Please specify a {typ} name.')

    directory: str = os.path.join(
        os.path.join(config.cm_dir,
                     'commands' if typ == 'command' else 'middleware'),
        args[0]) if 'here' not in flags else os.path.join(
            client.path, args[0])

    if os.path.exists(directory):
        raise errors.Exists(f'{typ.capitalize()} already exists.')

    os.makedirs(directory)

    with open(os.path.join(directory, 'main.py'), 'w') as f:
        f.write(
            BASE.replace('{pkg_name}', args[0]) if 'middleware' not in
            flags else MIDDLEWARE)

    await client.reload()
    utils.success(f'Successfully initalized {typ} "{args[0]}".')
