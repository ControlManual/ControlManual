from typing import List, Dict
from ...client import Client  # Only used for intellisense, will not work if this file is run manually.

HELP: str = 'Print a message.'
USAGE: str = '<message>'
ARGS: dict = {'message': 'Message to print.'}
FLAGS: dict = {'with-success': 'Prints the message as a success.'}
PACKAGE: str = 'builtin'


async def run(raw: str, args: List[str], kwargs: Dict[str, str],
              flags: List[str], client: Client):
    utils = client.utils
    console = client.console

    target = console.print if 'with-success' not in flags else utils.success
    target(' '.join(args))
