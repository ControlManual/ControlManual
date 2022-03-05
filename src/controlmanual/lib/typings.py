from __future__ import annotations
from typing import (
    Any,
    AsyncIterator,
    Callable,
    Dict,
    List,
    Literal,
    Optional,
    Protocol,
    Tuple,
    TypedDict,
    Union,
)

ReadTheme = Literal[
    "vim",
    "fruity",
    "inkpot",
    "native",
    "solarized-dark",
    "paraiso-dark",
    "stata-dark",
    "monokai",
    "gruvbox-dark",
    "dracula",
    "one-dark",
    "material",
    "zenburn",
    "paraiso-light",
    "perldoc",
    "friendly_grayscale",
    "friendly",
    "gruvbox-light",
    "manni",
    "solarized-light",
    "tango",
    "emacs",
    "vs",
    "trac",
    "stata-light",
    "stata",
    "rainbow_dash",
    "pastie",
    "murphy",
    "lovelace",
    "igor",
    "colorful",
    "borland",
    "autumn",
    "arduino",
    "agol_nu",
    "agol",
    "abap",
    "rtt",
    "xcode",
    "sas",
    "bw",
    "default",
]


class CommandCallable(Protocol):
    async def __call__(
        self,
        raw: str,
        args: List[str],
        kwargs: Dict[str, str],
        flags: List[str],
        client,
    ) -> CommandResponse:
        ...


class CommandIterator(Protocol):
    def __call__(
        self,
        raw: str,
        args: List[str],
        kwargs: Dict[str, str],
        flags: List[str],
        client,
    ) -> AsyncIterator[Any]:
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
    lowercase: bool
    read_theme: ReadTheme

class Function(TypedDict):
    arguments: List[str]
    script: List[str]
    defined: bool

Commands = Dict[str, Union[Command, BinaryCommand]]
ParsedString = Tuple[List[str], Dict[str, str], List[str]]
CommandResponse = Union[Any, AsyncIterator[Any]]
Functions = Dict[str, Function]