from typing import List, Dict
from src import Client # Only used for intellisense, will not work if this file is run manually.
import os

HELP: str = 'Interact with the system terminal.'
USAGE: str = '<command>'
ARGS: dict = {'command': 'Command that will be run.'}
PACKAGE: str = 'builtin'

def run(raw: str, args: List[str], kwargs: Dict[str, str], flags: List[str], client: Client):
    utils = client.utils

    if not args:
        return utils.error('Please specify a command.')

    os.system(raw)
    utils.success('Ran command.')