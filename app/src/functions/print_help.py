from typing import Dict, Union
from types import ModuleType
from ..utils import *
from ..config import Config

def print_help(commands: Dict[str, Dict[str, Union[str, ModuleType]]]) -> None:

    start: str = f'{bright_blue}For more info on a command, use {bright_green}"help <command>"{reset}\n'
    print(start)
    config = Config()

    for i in commands:
        if 'exe' in commands[i]:
            if config.hide_exe_from_help:
                continue
            hlp = f'{bright_yellow}Executable File.{reset}'
        else:
            hlp = f'{green}{commands[i]["help"]} {bright_red}{commands[i]["warning"]}{reset}'


        print(f'{bright_green}{i.lower()}{reset} - {hlp}')
    
    print()