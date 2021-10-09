import os

BASE: str = """from typing import List, Dict
from src import Client # Only used for intellisense, will not work if this file is run manually.

HELP: str = 'Basic command.'
USAGE: str = '<message>'
ARGS: dict = {'message': 'Message to print.'}
FLAGS: dict = {'hello_world': 'Whether to print "Hello World".'}

def run(raw: str, args: List[str], kwargs: Dict[str, str], flags: List[str], client: Client):

    config = client.config
    utils = client.utils
    api = client.api
    objects = client.objects

    if not args:
        return utils.error('Please specify a message.')

    if 'hello_world' in flags:
        print("Hello World")
    else:
        print(args[0])
"""

MIDDLEWARE: str = '''from typing import List, Dict
from src import Client # Only used for intellisense, will not work if this file is run manually.

def run(command: str, raw: str, args: List[str], kwargs: Dict[str, str], flags: List[str], client: Client):
    print(f'Command "{command}" was run!')
'''

from typing import List, Dict
from src import Client # Only used for intellisense, will not work if this file is run manually.

HELP: str = 'Initalize a command or middleware.'
USAGE: str = '<name> [flags]'
ARGS: dict = {'name': 'Name of the command.'}
FLAGS: dict = {'here': 'Initalizes the command in the current directory.', 'middleware': 'Initalizes middleware instead of a command.'}


def run(raw: str, args: List[str], kwargs: Dict[str, str], flags: List[str], client: Client):
    utils = client.utils
    config = client.config

    typ: str = 'command' if 'middleware' not in flags else 'middleware'

    if len(args) == 0:
        return print(f'{utils.bright_red}Please specify a {typ} name.{utils.reset}')

    directory: str = os.path.join(
        os.path.join(
            config.cm_dir,
            'commands' if typ == 'command' else 'middleware'
        ),
        args[0]
    ) if 'here' not in flags else os.path.join(client.path, args[0])

    if os.path.exists(directory):
        return print(f'{utils.bright_red}{typ.capitalize()} already exists.{utils.reset}')

    os.makedirs(directory)

    with open(os.path.join(directory, 'main.py'), 'w') as f:
        f.write(BASE if 'middleware' not in flags else MIDDLEWARE)

    client.reload()
    utils.success(f'Successfully initalized {typ} "{args[0]}".')