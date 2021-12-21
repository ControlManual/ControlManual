from typing import List, Dict
from ...client import Client  # Only used for intellisense, will not work if this file is run manually.
import os

HELP: str = 'Interact with the system terminal.'
USAGE: str = '<command>'
ARGS: dict = {'command': 'Command that will be run.'}
PACKAGE: str = 'builtin'


async def run(raw: str, args: List[str], kwargs: Dict[str, str],
              flags: List[str], client: Client):
    utils = client.utils
    errors = client.errors

    if not args:
        raise errors.NotEnoughArguments('Please specify a command.')

    os.system(raw)
    utils.success('Ran command.')
