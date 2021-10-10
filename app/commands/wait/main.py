from typing import List, Dict
from src import Client # Only used for intellisense, will not work if this file is run manually.
import time as t

HELP: str = 'Wait a certain amount of time.'
USAGE: str = '<amount> [flags]'
ARGS: dict = {'amount': 'Amount of time to wait (in seconds).'}
FLAGS: dict = {'minutes': 'Wait in minutes.', 'milliseconds': 'Wait in milliseconds.'}
PACKAGE: str = 'builtin'

def run(raw: str, args: List[str], kwargs: Dict[str, str], flags: List[str], client: Client):

    utils = client.utils

    if not args:
        return utils.error('Please specify an amount of time.')

    try:
        time: int = int(args[0])
    except ValueError:
        return utils.error('Please specify a valid amount of time.')

    if 'milliseconds' in flags:
        time = time / 1000
    
    if 'minutes' in flags:
        time = time * 60
    
    t.sleep(time)