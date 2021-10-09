from typing import List, Dict
from src import Client # Only used for intellisense, will not work if this file is run manually.

HELP: str = 'Clears the screen.'
USAGE: str = ''
ARGS: dict = None
FLAGS: dict = None

def run(raw: str, args: List[str], kwargs: Dict[str, str], flags: List[str], client: Client):
    client.utils.clear()