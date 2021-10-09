from typing import List, Dict
from src import Client # Only used for intellisense, will not work if this file is run manually.
import sys

HELP: str = 'Exit the instance.'
USAGE: str = '<code>'
ARGS: dict = {'code': 'Exit code of the program. Set to 0 if left blank.'}
FLAGS: dict = {}

def run(raw: str, args: List[str], kwargs: Dict[str, str], flags: List[str], client: Client):
    utils = client.utils

    if not args:
        code: int = 0
    else:
        try:
            code: int = int(args[0])
        except ValueError:
            return utils.error('Please enter a valid status code.')
        
    sys.exit(code)