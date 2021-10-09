from typing import Tuple, Dict, List
import shlex
from ..utils import *

def parse(raw: str) -> Tuple[List[str], Dict[str, str], List[str]]:
    """Function for parsing the input into different items."""
    try:
        split: List[str] = shlex.split(raw)
    except ValueError:
        error('Invalid quotation in arguments.')

        return [], {}, []

    kwargs: Dict[str, str] = {}
    flags: List[str] = []
    args: List[str] = []

    for i in split:
        if ('=' in i) and (len(i) >= 3) and (not i[i.index('=') - 1] == r'\\') and (i.count('=') == 1): # a bit messy but probably the best way to do this
            split: List[str] = i.split('=')
            kwargs[split[0]] = split[1]

        elif i.startswith('--'):
            flags.append(i[2:])
        
        else:
            args.append(i)



    return args, kwargs, flags
