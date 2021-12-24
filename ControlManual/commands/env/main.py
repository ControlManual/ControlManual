from typing import List, Dict
from src.client import Client  # Only used for intellisense, will not work if this file is run manually.
import os

HELP: str = 'Perform environment variable operations.'
USAGE: str = '<operation> [name] [value]'
ARGS: dict = {'message': 'Message to print.'}
ARGS_HELP: dict = {
    'value': {
        'required_when': 'argument "operation" is "set".'
    },
    'operation': {
        'valid_values': ['get', 'set', 'delete', 'list']
    },
    'name': {
        'not_required_when': 'argument "operation" is "list".'
    }
}
FLAGS: dict = {}
PACKAGE: str = 'builtin'

async def run(raw: str, args: List[str], kwargs: Dict[str, str], flags: List[str], client: Client):
    errors = client.errors
    console = client.console

    if not args:
        raise errors.NotEnoughArguments('Please specify an operation.')

    operation = args[0]

    if operation in ['get', 'set', 'delete']:
        if len(args) < 2:
            raise errors.NotEnoughArguments('Please specify a value.')

        name = args[1]
    else:
        name = '' # for type safety

    if operation == 'get':
        var = os.environ.get(name)

        if not var:
            raise errors.NotExists("Please specify a valid environment variable.")
        
        return console.print(var)
    
    if operation == 'set':
        if len(args) < 3:
            raise errors.NotEnoughArguments('Please specify a value.')
        
        os.environ[name] = args[2]
        return console.success(f'Set value for environment variable "{name}".')
    
    if operation == 'delete':
        if not os.environ.get(name):
            raise errors.NotExists("Please specify a valid environment variable.")
        
        os.environ.pop(name)
        return console.success(f'Deleted environment variable "{name}"')

    if operation == 'list':
        return console.print(' '.join([f'[primary]{i}[/primary]' for i in os.environ]))

    raise errors.InvalidArgument('Please specify a valid operation.')