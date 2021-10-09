from typing import List, Dict
from src import Client # Only used for intellisense, will not work if this file is run manually.

HELP: str = 'Set the window title.'
USAGE: str = '<title> [flags]'
ARGS: dict = {'title': 'Text to set as the window title.'}
FLAGS: dict = {'prefix': 'Adds the "Control Manual - " prefix to the title.'}

def run(raw: str, args: List[str], kwargs: Dict[str, str], flags: List[str], client: Client):
    utils = client.utils

    if not args:
        return print(f'{utils.bright_red}Please specify a window title{utils.reset}')

    if 'prefix' in flags:
        title: str = f'Control Manual - {args[0]}'
    else:
        title: str = args[0]

    utils.title(title)
    utils.success('Successfully updated window title.')