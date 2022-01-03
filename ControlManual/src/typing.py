from typing import TypedDict, Optional, AsyncIterator, Dict, Union, Tuple, List, Literal, Callable, Protocol

class CommandCallable(Protocol):
    async def __call__(self, raw: str, args: List[str], kwargs: Dict[str, str], flags: List[str], client):
        ...

class Command(TypedDict):
    entry: CommandCallable
    cmd_help: str
    usage: str
    package: str
    warning: str
    args: dict
    flags: dict
    args_help: dict
    live: bool
    iterator: Optional[Callable[..., AsyncIterator]]

class BinaryCommand(TypedDict):
    exe: str

class CommandErrors(TypedDict):
    unknown_command: str
    command_error: str
    function_open: str
    function_not_open: str
    function_undefined: str
    permission_error: str

class Config(TypedDict):
    input_sep: str
    flag_prefix: str
    colorize: str
    use_path_env: str
    hide_exe_from_help: str
    aliases: Dict[str, str]
    comments: List[str]
    functions: List[str]
    help_command: str
    seperator: str
    errors: CommandErrors
    truecolor: bool
    basic: bool
    basic_input: bool
    log_level: Literal["DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL"]

Commands = Dict[str, Union[BinaryCommand, Command]]
ParsedString = Optional[Tuple[List[str], Dict[str, str], List[str]]]