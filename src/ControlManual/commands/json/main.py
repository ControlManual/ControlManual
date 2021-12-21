from typing import List, Dict, Any
from ...client import Client  # Only used for intellisense, will not work if this file is run manually.
import json

HELP: str = 'Perform operations with a json file.'
USAGE: str = '<file> <operation> [key] [value]'
ARGS: dict = {
    'file': 'JSON file to perform operation on.',
    'operation': 'Operation to run on the file.',
    'key': 'JSON key to perform the operation on.'
}
ARGS_HELP: dict = {
    'file': {
        'type': 'Path'
    },
    'operation': {
        'valid_values': ['get', 'set', 'remove', 'read']
    },
    'key': {
        'not_required_when': 'Argument "operation" is "read".'
    }
}
FLAGS: dict = {
    'no-string':
    'Insert a JSON value without surrounding the argument in quotes.'
}
PACKAGE: str = 'builtin'


async def run(raw: str, args: List[str], kwargs: Dict[str, str],
              flags: List[str], client: Client):

    utils = client.utils
    static = client.static
    errors = client.errors

    if not args:
        raise errors.NotEnoughArguments('Please specify a file.')

    path: str = utils.get_path(client.path, args[0], file=True)
    if not path:
        raise errors.NotExists(f'File "{args[0]}" does not exist.')

    file = static.JSONFile(path)

    if len(args) < 2:
        raise errors.NotEnoughArguments('Please specify an operation.')

    if args[0] == 'get':
        if len(args) < 3:
            raise errors.NotEnoughArguments('Please specify a key.')
        raw = file.raw

        if not args[2] in raw:
            raise errors.NotExists(f'Key "{args[2]}" does not exist.')

        return utils.success(raw[args[2]])

    if args[1] == 'set':
        if len(args) < 3:
            raise errors.NotEnoughArguments('Please specify a value.')

        if not 'no-string' in flags:
            args[3] = f'"{args[3]}"'

        try:
            value: dict = json.loads(args[3])
        except json.decoder.JSONDecodeError:
            raise errors.InvalidArgument('Please specify a valid JSON string.')

        file.set_value(args[2], value)

        return utils.success(f'Key "{args[2]}" was set to {args[3]}.')

    if args[1] == 'remove':
        if len(args) < 3:
            return utils.error('Please specify a key.')

        if not args[2] in file.raw:
            return utils.error(f'Key "{args[2]}" does not exist.')

        file.erase_value(args[3])

        return utils.success(f'Key "{args[2]}" was deleted.')

    if args[1] == 'read':
        return utils.success(utils.format_json(file.raw))

    return utils.error(f'Please specify a valid operation.')
