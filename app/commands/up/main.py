from typing import List, Dict
from src import Client # Only used for intellisense, will not work if this file is run manually.
from pathlib import Path

HELP: str = 'Set the path to current parent directory.'
USAGE: str = '<amount>'
ARGS: dict = {'amount': 'Amount to go up by (defaults to 1).'}
FLAGS: dict = {}
PACKAGE: str = 'builtin'

def run(raw: str, args: List[str], kwargs: Dict[str, str], flags: List[str], client: Client):

    utils = client.utils

    if args == []:
        amount: int = 1
    else:
        amount = args[0]

        try:
            amount = int(amount)
        except ValueError:
            return utils.error('Invalid number for amount.')
    
    for i in range(amount):
        path = Path(client.path)
        client.change_path(path.parent)

    utils.success('Successfully updated directory.')

