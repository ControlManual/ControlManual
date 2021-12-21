from typing import List, Dict
from ...client import Client  # Only used for intellisense, will not work if this file is run manually.
from pathlib import Path

HELP: str = 'Set the path to current parent directory.'
USAGE: str = '<amount>'
ARGS: dict = {'amount': 'Amount to go up by.'}
ARGS_HELP: dict = {
    'amount': {
        'type': 'Number',
        'when_unspecified': 'Defaults to 1.'
    }
}
PACKAGE: str = 'builtin'


async def run(raw: str, args: List[str], kwargs: Dict[str, str],
              flags: List[str], client: Client):

    utils = client.utils
    errors = client.errors

    if args == []:
        amount: int = 1
    else:
        amount = args[0]

        try:
            amount = int(amount)
        except ValueError:
            raise errors.InvalidArgument('Invalid number for amount.')

    for i in range(amount):
        path = Path(client.path)
        client.change_path(path.parent)

    return utils.success('Successfully updated directory.')
