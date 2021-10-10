from typing import List, Dict
from src import Client # Only used for intellisense, will not work if this file is run manually.
import os

HELP: str = 'Interact with the system terminal.'
USAGE: str = '<command>'
ARGS: dict = {'command': 'Command that will be run.'}
FLAGS: dict = {}
PACKAGE: str = 'builtin'

def run(raw: str, args: List[str], kwargs: Dict[str, str], flags: List[str], client: Client):
    os.system(raw)
    client.utils.success('Sucessfully ran command.')