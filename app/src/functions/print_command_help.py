from types import ModuleType
from typing import Dict, Union
from ..utils import run_exe, error, success, make_success
from ..theme import *

def make_str(commands: dict, command: str, key: str, prefix: str = '') -> str:
    item = commands[command][key]

    if item:
        return prefix + item + '\n'
    else:
        return f'{warning}No {key}.\n{reset}'


def print_command_help(commands: Dict[str, Dict[str, Union[str, ModuleType]]], command: str) -> None:
    if not (command in commands):
        return error(f'Command does not exist.')
    
    if 'exe' in commands[command]:
        return print(run_exe(commands[command]['exe']))


    cmd_help: str = make_str(commands, command, 'help')
    usage: str = make_str(commands, command, 'usage', f'{secondary}{command} {primary}')
    package: str = make_str(commands, command, 'package')
    args_dict: dict = commands[command]['args']
    flags_dict: dict = commands[command]['flags']
    args = flags = ''

    if (args_dict is None) or (args_dict == {}): # TODO: optimize
        args += f'{warning}No arguments.\n'
    else:
        if args_dict == {}:
            args += make_str(commands, command, 'args')
        else:
            for i in args_dict:
                args += f'{primary}{i}{reset} - {secondary}{args_dict[i]}{reset}\n'

    if (flags_dict is None) or (flags_dict == {}):
        flags += f'{warning}No flags.\n'
    else:
        if flags_dict == {}:
            flags += make_str(commands, command, 'flags')
        else:
            for i in flags_dict:
                flags += f'{primary}{i}{reset} - {secondary}{flags_dict[i]}{reset}\n'
            
    
    print(f'''{primary}{cmd_help}{reset}
{important}Package{reset}
{primary}{package}{reset}
{important}Usage{reset}
{usage}{reset}
{important}Args
{args}
{important}Flags
{flags}{reset}
{important}For more information on a certain argument or flag, use {primary}"help {command} <argument>"{reset}
''')
