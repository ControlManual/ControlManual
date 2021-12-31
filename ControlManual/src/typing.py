from typing import TypedDict, Optional, AsyncIterator, Dict, Union, Tuple, List, Coroutine, Callable, TypeVar


class Command(TypedDict):
    entry: Callable[..., Coroutine]
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

Commands = Dict[str, Union[BinaryCommand, Command]]
ParsedString = Optional[Tuple[List[str], Dict[str, str], List[str]]]