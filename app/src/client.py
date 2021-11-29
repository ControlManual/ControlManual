from .functions import *
from typing import Dict, Union, Callable, List, Any
from types import ModuleType
from pathlib import Path
from .config import Config, cm_dir, config_path
import os
from . import utils, api, static, error as command_errors
from .utils import *
from .api import *
import colorama
from threading import Thread
from .console import console, ConsoleWrapper
import platform
import os
import psutil
from .error import *
import getpass
import datetime
import distro
import socket

class Reload:
    """Blank object used to reload the instance."""
    pass

def threaded(client: "Client") -> None:
    """Function ran on seperate thread when initalized."""
    client._connected = is_online()

class Client:
    """Base class for running Control Manual."""
    def __init__(self, version: dict) -> None:
        """Base class for running Control Manual."""

        self._config = Config()
        self._reset: bool = False
        self._version: str = version['string']
        self._path: Path = Path().home()
        self._functions: Dict[str, Dict[str, Union[str, List[str]]]] = {}
        self._current_function: str = None
        self._function_open: bool = False
        self._toggled_output: bool = True
        self._origin: Path = self._path
        self._actual_functions: Dict[str, Callable] = {
            'resp': self.get_command_response
        }
        self._variables: Dict[str, str] = {
            'path': str(self._path),
            'config': self.config_path,
            'cmdir': self.cm_dir
        }
        self._aliases: Dict[str, str] = {}
        for i in self._config.aliases:
            self._aliases[i] = self.load_variables(self._config.aliases[i])

        colorama.init(convert = os.name == 'nt') # enables ascii stuff
        console.clear(), title('Control Manual')
        
        self._connected = False
        Thread(target = threaded, args = [self]).start()

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
    def static(self) -> static:
        """Module containing standalone objects."""
        return static

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
    def vals(self, key: Any, value: Any) -> None:
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
    
    def add_alias(self, alias: str, value: str) -> None:
        """Function for adding command aliases."""
        self._aliases[alias] = self.load_variables(value)

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

    @property
    def console(self) -> ConsoleWrapper:
        """Console object."""
        return console
    
    def add_variable(self, name: str, value: str) -> None:
        """Function for adding a variable to the instance."""
        self._variables[name] = value
    
    def change_path(self, path: Path) -> None:
        """Function for changing the current path."""
        self._path = path
    
    def invoke_reset(self) -> None:
        """Tell the Control Manual instance to reset after the current command has finished. Only works when run via the main file."""
        self._reset = True

    def load_variables(self, data: str) -> str:
        """Function for loading variables into a string."""
        for i in self._variables:
            data: str = data.replace('{' + i + '}', self._variables[i])
        
        return data
    
    def load_variables_dynamic(self, data: str, variables: dict) -> str:
        """Function for loading variables into a string."""
        for i in variables:
            data: str = data.replace('{' + i + '}', variables[i])
        
        return data

    def _format_string(self, collection: dict, text: str) -> str:
        for key, value in collection.items():
            text = text.replace('{' + key + '}', value)
        
        return text

    @property
    def actual_functions(self) -> Dict[str, Callable]:
        """Dictionary of functions that can be called in the command line."""
        return self._actual_functions
    
    @property
    def errors(self) -> command_errors:
        """Errors to raise in commands."""
        return command_errors

    @property
    def error_map(self) -> dict:
        """Map of errors and their corresponding metadata."""
        return {
            InvalidArguments: 5005,
            Other: 5006,
            NotEnoughArguments: 5007,
            Exists: 5008,
            NotExists: 5009,
            InvalidArgument: 5010,
            APIError: 5011,
            NothingChanged: 5012
        }
    
    def get_command_response(self, args: List[str]) -> None:
        return get_resp(self.run_command, ' '.join(args))

    def start(self, filename: str) -> Union[None, Reload]:
        """Start the main loop."""
        while True:
            if filename:
                if not os.path.exists(filename):
                    error('Could not find file.')
                else:
                    with open(filename) as f:
                        for i in f.read().split('\n'):
                            self.run_command(i)
            
            inp: str = f'[white on black]{self._path} [primary]>>[/primary] '

            if not os.path.exists(self._path):
                static.static_error("path does not exist.")

            console.set_dir(self._path)
            memory_raw = psutil.virtual_memory()
            memory = f'{memory_raw.used // 1000000}mB / {memory_raw.total // 1000000}mB'

            disk_raw = psutil.disk_usage('/')
            disk = f'{disk_raw.used // 1000000000}gB / {disk_raw.total // 1000000000}gB'

            battery = psutil.sensors_battery()
            system = distro.name(pretty = True) if platform.system() == 'Linux' else f'{platform.system()} {platform.release()} {platform.version()}'
            console.set_info(f"""User: [primary]{getpass.getuser()}[/primary]
OS: [primary]{system}[/primary]
Machine: [primary]{platform.machine()}[/primary]
System Time: [primary]{datetime.datetime.now().strftime('%H:%M:%S')}[/primary]
CPU Usage: [primary]{psutil.cpu_percent()}%[/primary]
Memory: [primary]{memory}[/primary]
Disk: [primary]{disk}[/primary]
Battery: [primary]{str(battery.percent)}%[/primary]
Computer Name: [primary]{platform.node()}[/primary]
""")
            command: str = console.take_input(inp)

            console.clear_panel("exceptions")
            console.clear_panel("data")
            command = command.replace(r'\n', '\n')
    
            self.run_command(command)
            
            if self._reset:
                return Reload

            filename = False
    
    def run_command(self, command: str) -> None:
        """Function for running a command."""
        config = self._config
        errors = config.errors

        for i in config.comments:
            if command.startswith(i):
                return
            
        cmds = command.split(config.seperator)
        for comm in cmds:
            comm = self.load_variables(comm)

            split: List[str] = comm.split(' ')
            if not any(split):
                continue

            while not split[0]:
                split.pop(0)

            raw_args = ' '.join(split[1:]) # unsplit string of arguments
            cmd = split[0]
            while True:
                if cmd in self.aliases:
                    spl = self.aliases[cmd].split(' ')
                    cmd = spl[0]
                    if len(spl) > 1:
                        excess: str = " ".join(spl[1:])
                        raw_args = excess + ' ' + raw_args
                else:
                    break

            args, kwargs, flags = parse(raw_args)
            cmd: str = cmd.lower()

            for index, i in enumerate(args):
                for key, value in self.actual_functions.items():
                    while True:
                        find = "{" + key + "("
                        
                        if find in i:

                            ind = i.index(find)
                            try:
                                end: int = i.index(')}')
                            except ValueError:
                                end: int = -1
                            
                            text: str = i[ind + len("{" + key) + 1:end]

                            params = parse(text)[0]
                            replace = value(params)
                            args[index] = i = args[index].replace(f'{find}{text})' + '}', replace)
                        else:
                            break
                

            crfn: str = self.current_function
            COMMANDS: Dict[str, Dict[str, Union[ModuleType, str]]] = self.commands
            
            for fn in self.middleware:
                fn(cmd, raw_args, args, kwargs, flags, COMMANDS)

            if cmd == config.functions[0]:
                if crfn:
                    if self._function_open:
                        utils.error(errors['function_open'])
                    else:
                        self._function_open = True
                else:
                    utils.error(errors['function_undefined'])

                return
            
            if cmd == config.functions[1]:
                if crfn:
                    if not self._function_open:
                        utils.error(errors['function_not_open'])
                    else:
                        self.functions[self.current_function]['defined'] = True
                        self._function_open = False
                        self._current_function = None
                else:
                    utils.error(errors['function_undefined'])

                return

            if self._function_open:
                if not command == config.functions[1]:
                    return self._functions[crfn]['script'].append(command)

            if cmd == config.help_command:
                if args:
                    if len(args) > 1:
                        print_argument_help(COMMANDS, args[0], args[1])
                    else:
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

                    return console.print(
                        run_exe(COMMANDS[cmd]['exe'],
                        ''.join(
                            args[0:]
                            )
                        )
                    )

                runner: Callable = COMMANDS[cmd]['entry']

                try:
                    call = runner(raw_args, args, kwargs, flags, self)
                except Exception as e:
                    emap = self.error_map

                    if type(e) in emap:
                        console.error(str(e))
                        return console.set_data(error_meta(emap[type(e)], 'explicit'))

                    if isinstance(e, PermissionError):
                        code: int = 5003
                        error(errors["permission_error"])
                    else:
                        code: int = 5002
                        failure: str = errors["command_error"].replace('{cmd}', cmd)
                        console.error(failure)
                    
                    console.show_exc(e)
                    return console.set_data(error_meta(code))
                
                try:
                    m: dict = call[-1] if not isinstance(call, dict) else call
                    console.set_data(m)
                except: # a lot of things could go wrong so i wont specify the error
                    console.set_data(error_meta(5004))

            else:
                console.set_data(
                    error_meta(5001)
                )
                error(errors['unknown_command'])