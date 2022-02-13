from ...constants import NotEnoughArguments, InvalidArgument
from typing import Literal, List, Optional, Dict, Iterator, Union, NoReturn
from .get_path import get_path, get_path_inverted
from pathlib import Path

ArgType = Literal['file', 'folder', 'string', 'number', 'newfile', 'newfolder']

def get_arg(args: List[str], index: int, arg_type: ArgType, current_path: Optional[Path] = None) -> Union[str, NoReturn]:
    """Function for getting and validating an argument."""
    if len(args) <= index:
        raise NotEnoughArguments('Insufficent arguments.')

    value = args[index]

    if arg_type == 'number':
        try:
            int(value)
        except ValueError:
            raise InvalidArgument('Invalid number.')

    if arg_type in ['file', 'folder', 'newfile', 'newfolder']:
        if not current_path:
            raise ValueError('current_path is required')
        
        files: list = ['file', 'newfile']
        is_new: bool = arg_type in ['newfolder', 'newfile']

        target = get_path_inverted if is_new else get_path
        tmp: Optional[str] = target(current_path, value, arg_type in files)

        if not tmp:
            raise InvalidArgument(f'{arg_type.capitalize()} {"already exists" if is_new else "not found"}.')

        value = tmp

    return value

def get_args(args: List[str], amount: int, argument_types: Dict[int, ArgType] = {}, current_path: Optional[Path] = None) -> Union[Iterator[str], NoReturn]:
    """Function for getting and validating arguments."""

    for i in range(amount):
        yield get_arg(args, i, 'string' if i not in argument_types else argument_types[i], current_path)