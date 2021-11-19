from typing import List, Dict
from src import Client # Only used for intellisense, will not work if this file is run manually.
import os

HELP: str = 'Run operations on packages.'
USAGE: str = '<operation> [package]'
ARGS: dict = {'operation': 'Operation to run.', 'package': 'Name of the package, unneeded if operation is "list".'}
FLAGS: dict = {}
PACKAGE: str = 'builtin'

def run(raw: str, args: List[str], kwargs: Dict[str, str], flags: List[str], client: Client):

    utils = client.utils
    api = client.api

    if not args:
        return utils.error('Please specify an operation.')

    if args[0] == 'add':
        if len(args) < 2:
            return utils.error('Please specify a package name.')
        
        commands_path: str = os.path.join(client.cm_dir, 'commands')
        temp_path: str = os.path.join(commands_path, f'temp_package_{args[1]}.zip')

        if not api.download_package(args[1], temp_path, commands_path):
            return utils.error('Please specify a valid package.')
        else:
            os.remove(temp_path)
            return utils.success(f'Successfully downloaded package "{args[1]}"')

    if args[0] == 'list':
        commands = client.commands
        packages = set()

        for i in commands:
            current_package = commands[i]['package']
            packages.add(current_package)
        
        for i in packages:
            utils.success(i)
        
        return
    
    utils.error('Please specify a valid operation.')
        
