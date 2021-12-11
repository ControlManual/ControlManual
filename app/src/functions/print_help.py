from typing import Dict, Union
from types import ModuleType
from ..config import Config
from ..console import console

def print_help(commands: dict) -> None:
    config = Config()

    for i in commands:
        if 'exe' in commands[i]:
            if config.hide_exe_from_help:
                continue
            hlp = f'[danger]Executable File.[/danger]'
        else:
            hlp = f'[secondary]{commands[i]["help"]}[/secondary] [danger]{commands[i]["warning"]}[/danger]'


        console.print(f'[primary]{i.lower()}[/primary] - {hlp}')
    
    console.print(f'\n[important]For more info on a command, use [/important][primary]"help <command>"[/primary]\n')