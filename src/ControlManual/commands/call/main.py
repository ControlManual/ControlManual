from typing import List, Dict
from ...client import Client  # Only used for intellisense, will not work if this file is run manually.

HELP: str = 'Call a function.'
USAGE: str = '<function> [arguments]'
ARGS: dict = {
    'functions': 'Name of the function.',
    'arguments': 'Arguments to pass into the function.'
}
PACKAGE: str = 'builtin'


async def run(raw: str, args: List[str], kwargs: Dict[str, str],
              flags: List[str], client: Client):

    errors = client.errors

    if not args:
        raise errors.NotEnoughArguments('Please specify a function name.')

    fn = client.functions.get(args[0])

    if not fn:
        raise errors.InvalidArgument('Please specify a valid function.')

    arguments = fn['arguments']
    params = args[1:]

    if not fn['defined']:
        raise errors.NothingChanged(
            f'Function "{args[0]}" is defined, but does not have a body.')

    if not len(arguments) == len(params):
        raise errors.NotEnoughArguments('Missing function parameters.')

    for i in fn['script']:
        text = i
        variables: dict = {}

        for index, value in enumerate(arguments):
            variables[value] = params[index]

        text = await client._format_string(variables, text)
        await client.run_command(text)
