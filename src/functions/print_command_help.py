from types import ModuleType
from typing import Dict, Union
from ..utils import *

def make_str(commands: dict, command: str, key: str) -> str:
    item = commands[command][key]
    if (item) or (item == ''):
        return item + '\n'
    else:
        return f'{bright_red}No {key} specified.{reset}'

def make_dict(data: dict) -> dict:
    """Will make later."""
    pass


def print_command_help(commands: Dict[str, Dict[str, Union[str, ModuleType]]], command: str) -> None:

    if not (command in commands):
        return print(f'{bright_red}Command does not exist.{reset}')
    
    if 'exe' in commands[command]:
        return print(run_exe(commands[command]['exe']))


    cmd_help: str = make_str(commands, command, 'help')
    usage: str = make_str(commands, command, 'usage')
    args_dict: dict = commands[command]['args']
    flags_dict: dict = commands[command]['flags']
    args = flags = ''

    if (args_dict is None) or (args_dict == {}): # TODO: optimize
        args += f'{bright_red}No arguments.\n'
    else:
        if args_dict == {}:
            args += make_str(commands, command, 'args')
        else:
            for i in args_dict:
                args += f'{bright_green}{i}{reset} - {green}{args_dict[i]}{reset}\n'

    if (flags_dict is None) or (flags_dict == {}):
        flags += f'{bright_red}No flags.\n'
    else:
        if flags_dict == {}:
            flags += make_str(commands, command, 'flags')
        else:
            for i in flags_dict:
                flags += f'{bright_green}{i}{reset} - {green}{flags_dict[i]}{reset}\n'
            
    
    print(f'''{bright_green}{cmd_help}{reset}
{bright_blue}Usage{reset}
{bright_green}{command} {normal}{usage}{reset}
{bright_blue}Args
{args}
{bright_blue}Flags
{flags}{reset}''')
