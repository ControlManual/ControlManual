from typing import List, Dict
from ...client import Client  # Only used for intellisense, will not work if this file is run manually.

HELP: str = 'Set the window title.'
USAGE: str = '<title> [flags]'
ARGS: dict = {'title': 'Text to set as the window title.'}
FLAGS: dict = {'prefix': 'Adds the "Control Manual - " prefix to the title.'}
PACKAGE: str = 'builtin'


async def run(raw: str, args: List[str], kwargs: Dict[str, str],
              flags: List[str], client: Client):
    utils = client.utils
    errors = client.errors

    if not args:
        raise errors.NotEnoughArguments(f'Please specify a window title')

    if 'prefix' in flags:
        title: str = f'Control Manual - {args[0]}'
    else:
        title: str = args[0]

    utils.title(title)
    return utils.success('Successfully updated window title.')
