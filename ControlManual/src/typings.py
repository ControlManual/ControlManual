from typing import TypedDict, Optional, AsyncGenerator, Dict, Callable, Tuple, List, Literal, Any, Protocol, Union

class CommandCallable(Protocol):
    async def __call__(self, raw: str, args: List[str], kwargs: Dict[str, str], flags: List[str], client):
        ...

class CommandIterator(Protocol):
    async def __call__(self, raw: str, args: List[str], kwargs: Dict[str, str], flags: List[str], client) -> AsyncGenerator[Any, None]:
        ...

class BaseCommand(TypedDict):
    help: Optional[str]
    usage: Optional[str]
    package: Optional[str]
    warning: str
    args: Optional[dict]
    flags: Optional[dict]
    args_help: Optional[dict]
    path: str

class Command(BaseCommand):
    entry: CommandCallable
    iterator: Optional[CommandIterator]
    is_binary: Literal[False]

class BinaryCommand(BaseCommand):
    entry: Callable
    iterator: Optional[Callable]
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

Commands = Dict[str, Union[Command, BinaryCommand]]
ParsedString = Tuple[List[str], Dict[str, str], List[str]]