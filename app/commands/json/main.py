from typing import List, Dict, Any
from src import Client # Only used for intellisense, will not work if this file is run manually.
import json

HELP: str = 'Perform operations with a json file.'
USAGE: str = '<file> <operation> [key] [value]'
ARGS: dict = {'file': 'JSON file to perform operation on.', 'operation': 'Whether to read the config, or get, set, or remove a value.', 'key': 'Key to perform the operation on (unneeded if operation is "read").', 'value': 'Value to perform the operation on (only needed if the operation is "set").'}
FLAGS: dict = {'no-string': 'Insert a JSON value without surrounding the argument in quotes.'}
PACKAGE: str = 'builtin'

def run(raw: str, args: List[str], kwargs: Dict[str, str], flags: List[str], client: Client):

    utils = client.utils
    objects = client.objects

    if len(args) < 1:
        return utils.error('Please specify an file.')

    path: str = utils.get_path(client.path, args[0], file = True)
    if not path:
        return utils.error(f'File "{args[0]}" does not exist.')

    file = objects.JSONFile(path)


    if len(args) < 2:
        return utils.error('Please specify an operation.')

    if args[0] == 'get':
        if len(args) < 3:
            return utils.error('Please specify a key.')
        raw = file.raw

        if not args[2] in raw:
            return utils.error(f'Key "{args[2]}" does not exist.')

        return utils.success(raw[args[2]])
    
    if args[1] == 'set':
        if len(args) < 3:
            return utils.error('Please specify a value.')
        
        if not 'no-string' in flags:
            args[3] = f'"{args[3]}"'

        try:
            value: dict = json.loads(args[3])
        except json.decoder.JSONDecodeError:
            return utils.error('Please specify a valid JSON string.')

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
        return utils.success(
            utils.format_json(
                file.raw
            )
        )
    
    return utils.error(f'Please specify a valid operation.')

