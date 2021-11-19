from typing import Dict, Union
from types import ModuleType
from ..theme import *
from ..config import Config

def print_help(commands: Dict[str, Dict[str, Union[str, ModuleType]]]) -> None:
    config = Config()

    for i in commands:
        if 'exe' in commands[i]:
            if config.hide_exe_from_help:
                continue
            hlp = f'{warning}Executable File.{reset}'
        else:
            hlp = f'{secondary}{commands[i]["help"]} {danger}{commands[i]["warning"]}{reset}'


        print(f'{primary}{i.lower()}{reset} - {hlp}')
    
    print(f'\n{important}For more info on a command, use {primary}"help <command>"{reset}\n')