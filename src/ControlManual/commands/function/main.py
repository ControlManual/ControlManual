from typing import List, Dict
from ...client import Client  # Only used for intellisense, will not work if this file is run manually.

HELP: str = 'Start a function declaration.'
USAGE: str = '<name> [arguments]'
ARGS: dict = {
    'name': 'Name of the function.',
    'arguments': 'Arguments attached to the function, seperated by a space.'
}
PACKAGE: str = 'builtin'


async def run(raw: str, args: List[str], kwargs: Dict[str, str],
              flags: List[str], client: Client):

    utils = client.utils
    errors = client.errors

    if not args:
        raise errors.NotEnoughArguments('Please specify a function name.')

    if client.current_function:
        raise errors.Collision(
            f'Please define body for function "{client.current_function}" before creating a new one.'
        )

    client._functions[args[0]] = {
        'arguments': args[1:] if len(args) > 1 else [],
        'script': [],
        'defined': False
    }
    client._current_function = args[0]

    return utils.success(f'Successfully created shallow function "{args[0]}".')
