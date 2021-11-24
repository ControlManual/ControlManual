from typing import List, Dict
from src import Client # Only used for intellisense, will not work if this file is run manually.

HELP: str = 'Print a message.'
USAGE: str = '<message>'
ARGS: dict = {'message': 'Message to print.'}
PACKAGE: str = 'builtin'

def run(raw: str, args: List[str], kwargs: Dict[str, str], flags: List[str], client: Client):
    client.utils.success(raw)
