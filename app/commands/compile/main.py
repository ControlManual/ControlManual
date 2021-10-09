from typing import List, Dict
from src import Client # Only used for intellisense, will not work if this file is run manually.
import os

HELP: str = 'Run a Control Manual script.'
USAGE: str = '<file>'
ARGS: dict = {'file': 'File to run.'}
FLAGS: dict = {}

def run(raw: str, args: List[str], kwargs: Dict[str, str], flags: List[str], client: Client):

    utils = client.utils

    if args == []:
        return utils.error('Please specify a file.')

    path: str = utils.get_path(client.path, args[0], file = True)
    
    if not path:
        return utils.error(f'File "{args[0]}" was not found.')
    
    with open(path) as f:
        read: str = f.read()

    for i in read.split('\n'):
        client.run_command(i)
    

