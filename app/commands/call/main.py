from typing import List, Dict
from src import Client # Only used for intellisense, will not work if this file is run manually.

HELP: str = 'Call a function.'
USAGE: str = '<function> [arguments]'
ARGS: dict = {'functions': 'Name of the function.', 'arguments': 'Arguments to pass into the function.'}
FLAGS: dict = {}
PACKAGE: str = 'builtin'

def run(raw: str, args: List[str], kwargs: Dict[str, str], flags: List[str], client: Client):

    utils = client.utils

    if args == []:
        return utils.error('Please specify a function name.')

    fn = client.functions.get(args[0])

    if not fn:
        return utils.error('Please specify a valid function.')
    
    for i in fn['script']:
        client.run_command(i)