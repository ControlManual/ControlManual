from typing import TypedDict, Optional, AsyncGenerator, Dict, Callable, Tuple, List, Literal, Any, Protocol, Union

class CommandCallable(Protocol):
    async def __call__(self, raw: str, args: List[str], kwargs: Dict[str, str], flags: List[str], client):
        ...

class CommandIterator(Protocol):
    async def __call__(self, raw: str, args: List[str], kwargs: Dict[str, str], flags: List[str], client) -> AsyncGenerator[Any, None]:
        ...

class Colors(TypedDict):
    red: str
    green: str
    dark_green: str

class BaseCommand(TypedDict):
    help: Optional[str]
    usage: Optional[str]
    package: Optional[str]
    warning: str
    args: Optional[Dict[str, str]]
    flags: Optional[Dict[str, str]]
    args_help: Optional[dict]
    path: str

class Command(BaseCommand):
    entry: CommandCallable
    iterator: Optional[CommandIterator]
    is_binary: Literal[False]

class BinaryCommand(BaseCommand):
    entry: Callable[..., Any]
    iterator: Optional[Callable[..., Any]]
    is_binary: Literal[True]

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
    colorize: bool
    use_path_env: bool
    hide_exe_from_help: bool
    aliases: Dict[str, str]
    comments: List[str]
    functions: List[str]
    help_command: str
    seperator: str
    errors: CommandErrors
    truecolor: bool
    log_level: Literal["DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL"]
    colors: Colors

Commands = Dict[str, Union[Command, BinaryCommand]]
ParsedString = Tuple[List[str], Dict[str, str], List[str]]