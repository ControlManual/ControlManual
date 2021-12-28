from typing import Coroutine, TypedDict, Any, Optional, AsyncGenerator, Dict, Union, Tuple, List

class Command(TypedDict):
    entry: Coroutine[Any, Any, Any]
    cmd_help: str
    usage: str
    package: str
    warning: str
    args: dict
    flags: dict
    args_help: dict
    live: bool
    iterator: Optional[AsyncGenerator]

class BinaryCommand(TypedDict):
    exe: str

Commands = Dict[str, Union[BinaryCommand, Command]]
ParsedString = Optional[Tuple[List[str], Dict[str, str], List[str]]]