from typing import List, Dict
from ...client import Client  # Only used for intellisense, will not work if this file is run manually.

HELP: str = 'Declare a variable.'
USAGE: str = '<name> <value>'
ARGS: dict = {
    'name': 'Name of the variable.',
    'value': 'Value of the variable.'
}
FLAGS: dict = None
PACKAGE: str = 'builtin'


async def run(raw: str, args: List[str], kwargs: Dict[str, str],
              flags: List[str], client: Client):

    utils = client.utils

    if len(args) <= 1:
        return utils.error('Please specify a name and value.')

    client.add_variable(args[0], args[1])
