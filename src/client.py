from .functions import *
from typing import Dict, Union, Callable, List, Any
from types import ModuleType
from .error import CMError
from pathlib import Path
from .config import Config, cm_dir, config_path
import os
from . import utils, api, objects
from .utils import *
from .api import *
import colorama
import requests

class Reload:
    """Blank object used to reload the instance."""
    pass

class Client:
    """Base class for running Control Manual."""
    def __init__(self, version: str) -> None:
        """Base class for running Control Manual."""

        self._config = Config()
        self._reset: bool = False
        self._version: str = version
        self._path: Path = Path().home()
        self._functions: Dict[str, Dict[str, Union[str, List[str]]]] = {}
        self._current_function: str = None
        self._function_open: bool = False
        self._toggled_output: bool = True
        self._origin: Path = self._path
        self._variables: Dict[str, str] = {
            'path': str(self._path),
            'config': self.config_path,
            'cmdir': self.cm_dir
        }
        self._aliases: Dict[str, str] = self._config.aliases

        if os.name == 'nt': # if the platform is windows
            colorama.init(convert=True) # enables ascii stuff for windows

        clear(), title('Control Manual')

        vers: str = f'{bright_green}{version}{reset}' if self._config.colorize else version

        print(f'Control Manual [{vers}] ')
        
        try:
            resp = requests.get("https://api.controlmanual.xyz/")

            if resp.status_code == 502:
                raise Exception()

            self._connected = True
        except:
            print(f'{bright_red}Failed to connect to the Control Manual API.{reset}\n')
            self._connected = False

        if (self._config.check_latest) and (self._connected):
            resp = requests.get("https://api.controlmanual.xyz/latest").json()
            
            if resp['version'] == version:
                print(f'{green}Running latest version!\n{reset}')
            else:
                print(f"{bright_yellow}Version {resp['version']} is available!\n{reset}")
        
        self.reload()
        self._vals: Dict[Any, Any] = {}
    
    def reload(self) -> None:
        """Function for reloading commands and middleware."""
        self._middleware: List[Callable] = load_middleware(
            join(
                self._config.cm_dir,
                'middleware'
            )
        )
        self._commands: Dict[str, Dict[str, Union[ModuleType, str]]] = load_commands(
            join(self._config.cm_dir, 'commands')
        )

    @property
    def cm_dir(self) -> str:
        """Top level directory of Control Manual."""
        return cm_dir
    
    @property
    def config_path(self) -> str:
        """Location of the config file."""
        return config_path

    @property
    def function_open(self) -> bool:
        """Whether a function is currently being edited."""
        return self._function_open
    
    @function_open.setter
    def set_function_open(self, value) -> None:
        self._function_open = value


    @property
    def objects(self) -> objects:
        """Module containing standalone objects."""
        return objects

    @property
    def functions(self) -> Dict[str, Dict[str, Union[str, List[str]]]]:
        """Dictionary of defined functions."""
        return self._functions
    
    @property
    def current_function(self) -> str:
        """Current function being edited."""
        return self._current_function
    
    @current_function.setter
    def set_current_function(self, value: str) -> None:
        self._current_function = value

    @property
    def vals(self) -> Dict[Any, Any]:
        """Namespace for commands and middleware to hold values."""
        return self._vals
    
    @vals.setter
    def change_vals(self, key: Any, value: Any) -> None:
        self._vals[key] = value

    @property
    def middleware(self) -> List[Callable]:
        """List of callables ran when a command is executed."""
        return self._middleware

    @property
    def connected(self) -> bool:
        """Whether the instance is connected to the Control Manual API."""
        return self._connected

    @property
    def config(self) -> Config:
        """Class for representing the JSON config."""
        return self._config

    @property
    def origin(self) -> Path:
        """Original path from initalization."""
        return self._origin

    @property
    def aliases(self) -> Dict[str, str]:
        """Command aliases."""
        return self._aliases
    
    def add_alias(self, alias, value) -> None:
        """Function for adding command aliases"""
        self._aliases[alias] = value

    @property
    def version(self) -> str:
        """Version of Control Manual."""
        return self._version

    @property
    def commands(self) -> Dict[str, ModuleType]:
        """Dictionary representation of commands."""
        return self._commands

    @property
    def path(self) -> Path:
        """Current path object."""
        return self._path
    
    @property
    def path_str(self) -> str:
        """Current path object as a string."""
        return str(self._path)
    
    @property
    def utils(self) -> utils: # for easier access to utils in commands
        """Utilities for commands."""
        return utils
    
    @property
    def api(self) -> api:
        """Functions regarding the API."""
        return api

    @property
    def variables(self) -> Dict[str, str]:
        """Dictionary representing variables."""
        return self._variables

    def add_variable(self, name: str, value: str) -> None:
        """Function for adding a variable to the instance."""
        self._variables[name] = value
    
    def change_path(self, path: Path) -> None:
        """Function for changing the current path."""
        self._path = path
    
    def invoke_reset(self) -> None:
        """Tell the Control Manual instance to reset after the current command has finished. Only works when run via the main file."""
        self._reset = True


    def start(self, filename) -> Union[None, Reload]:
        """Start the main loop."""
        while True:
            if filename:
                if not os.path.exists(filename):
                    error('Could not find file.')
                else:
                    with open(filename) as f:
                        for i in f.read().split('\n'):
                            self.run_command(i)
            
            inp: str = f'{self._path} {bright_blue}{self.config.input_sep}{reset} '
            if not os.path.exists(self._path):
                CMError('Path not found.', kill = True) \
                    .raise_exc()
            
            print(inp, end="")

            command: str = input()
            command = command.replace(r'\n', '\n')
    
            self.run_command(command)
            
            if self._reset:
                return Reload

            filename = False
            
    def run_command(self, command: str) -> None:
        """Function for running a command."""
        if command.startswith('//'):
            return
            
        cmds = command.split(';')
    
        for i in cmds:
            for i in self._variables:
                command: str = command.replace('{' + i + '}', self._variables[i])


            split: List[str] = command.split(' ')
            raw_args = ' '.join(split[1:]) # unsplit string of arguments
            cmd = split[0]
    
            if cmd in self.aliases:
                spl = self.aliases[cmd].split(' ')
                cmd = spl[0]
                if len(spl) > 1:
                    excess: str = " ".join(spl[1:])
                    raw_args = excess + raw_args

    
            args, kwargs, flags = parse(raw_args)

            cmd: str = cmd.lower()

            if cmd == '':
                continue

            if self._function_open:
                if not command == ')':
                    return self._functions[self.current_function]['script'].append(command)

            COMMANDS: Dict[str, Dict[str, Union[ModuleType, str]]] = self.commands
            
            for fn in self.middleware:
                fn(cmd, raw_args, args, kwargs, flags, COMMANDS)

            if cmd == "(":
                if self.current_function:
                    if self._function_open:
                        utils.error('Function is already open.')
                    else:
                        self._function_open = True
                else:
                    utils.error('No function currently defined.')

                return
            
            if cmd == ")":
                if self.current_function:
                    if not self._function_open:
                        utils.error('Function is not open.')
                    else:
                        self._function_open = False
                        self._current_function = None
                else:
                    utils.error('No function currently defined.')

                return
    

            if cmd == "help":
                if args:
                    print_command_help(COMMANDS, args[0])
                else:
                    print_help(COMMANDS)
                continue
            
            if cmd in COMMANDS:
                if 'exe' in COMMANDS[cmd]:
                    if len(args) == 0:
                        ext: list = ['', '']
                    
                    if len(args) == 1:
                        ext: list = ['']
                    
                    args.extend(ext)

                    return print(
                        run_exe(COMMANDS[cmd]['exe'],
                        ''.join(
                            args[0:]
                            )
                        )
                    )

                runner: Callable = COMMANDS[cmd]['entry']

                try:
                    runner(raw_args, args, kwargs, flags, self)
                except Exception as e:
                    if isinstance(e, PermissionError):
                        error('Control Manual does not have permission to do this.')
                    else:
                        failure: str = f'{bright_red}Exception occured when running command {bright_green}"{cmd}"{bright_red}.{reset}'
                        CMError(failure, e).raise_exc()
            else:
                msg = f'{bright_red}Unknown command. Use {normal}{green}"help"{bright_red} to see commands.{reset}'
                print(msg)