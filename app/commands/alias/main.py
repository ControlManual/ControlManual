from typing import List, Dict
from src import Client # Only used for intellisense, will not work if this file is run manually.

HELP: str = 'Add a command alias.'
USAGE: str = '<alias> <value>'
ARGS: dict = {'alias': 'Alias of the command.', 'value': 'String to run when alias is called.'}
FLAGS: dict = None
PACKAGE: str = 'builtin'

def run(raw: str, args: List[str], kwargs: Dict[str, str], flags: List[str], client: Client):
    utils = client.utils

    if len(args) < 2:
        return utils.error('Please specify an alias and value.')

    client.add_alias(args[0], args[1])

    return utils.success(f'"{args[0]}" was aliased to "{args[1]}"')