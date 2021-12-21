from typing import List, Dict
from ...client import Client  # Only used for intellisense, will not work if this file is run manually.

HELP: str = 'Add a command alias.'
USAGE: str = '<name> <value>'
ARGS: dict = {
    'name': 'Name of the alias.',
    'value': 'String to replace alias with when used.'
}
PACKAGE: str = 'builtin'


async def run(raw: str, args: List[str], kwargs: Dict[str, str],
              flags: List[str], client: Client):
    utils = client.utils
    errors = client.errors

    if len(args) < 2:
        raise errors.NotEnoughArguments('Please specify an alias and value.')

    await client.add_alias(args[0], args[1])
    return utils.success(f'"{args[0]}" was aliased to "{args[1]}"')
