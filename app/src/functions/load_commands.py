import os
import sys
import importlib
from typing import Dict, Union
from types import ModuleType
from ..config import Config

def load_commands(directory: str) -> Dict[str, Dict[str, Union[str, ModuleType]]]:
    """Function for creating the commands dict for the client."""
    config = Config()

    resp: Dict[str, Dict[str, Union[ModuleType, str]]] = {}
    
    sys.path.append(directory)
    for i in os.listdir(directory):
        if os.path.isfile(
            os.path.join(directory, i)
        ):
              
            resp[i[:-4]] = {'exe': os.path.join(directory, i)}
        else:
            command: ModuleType = importlib.import_module(f'commands.{i}.main')
            cmd_help: str = command.HELP if hasattr(command, 'HELP') else ''
            usage: str = command.USAGE if hasattr(command, 'USAGE') else ''
            package: str = command.PACKAGE if hasattr(command, 'PACKAGE') else ''
            warning: str = f'({command.WARNING})' if hasattr(command, 'WARNING') else ''
            args: dict = command.ARGS if hasattr(command, 'ARGS') else {}
            flags: dict = command.FLAGS if hasattr(command, 'FLAGS') else {}


            resp[i] = {
                'entry': command.run,
                'help': cmd_help,
                'warning': warning,
                'usage': usage, 
                'args': args, 
                'flags': flags,
                'package': package
            }
    
    if config.raw['use_path_env']:
        for i in os.environ['PATH'].split(';'):
            if os.path.exists(i):
                for x in os.listdir(i):
                    if not x[:-4] in resp:
                        if x.endswith('.exe'):
                            resp[x[:-4]] = {'exe': os.path.join(i, x)}
    
    return resp