from typing import List, Dict
from src import Client # Only used for intellisense, will not work if this file is run manually.

HELP: str = 'Start a function declaration.'
USAGE: str = '<name> [arguments]'
ARGS: dict = {'name': 'Name of the function.', 'arguments': 'Arguments attached to the function, seperated by a space.'}
FLAGS: dict = {}
PACKAGE: str = 'builtin'

def run(raw: str, args: List[str], kwargs: Dict[str, str], flags: List[str], client: Client):

    utils = client.utils

    if args == []:
        return utils.error('Please specify a function name.')

    client._functions[args[0]] = {
        'arguments': args[1:] if len(args) > 1 else None,
        'script': []
    }
    client._current_function = args[0]

    return utils.success(f'Successfully created shallow function "{args[0]}".')