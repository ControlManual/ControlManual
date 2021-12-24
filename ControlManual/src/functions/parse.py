from typing import Tuple, Dict, List
import shlex
from ..utils import *
from ..config import Config
from ..logger import log


async def parse(raw: str) -> Tuple[List[str], Dict[str, str], List[str]]:
    """Function for parsing the input into different items."""

    try:
        split: List[str] = shlex.split(raw)
    except ValueError:
        await log("split failed, returning blank for all items")
        error("Invalid quotation in arguments.")

        return [], {}, []

    kwargs: Dict[str, str] = {}
    flags: List[str] = []
    args: List[str] = []

    config = Config()
    for i in split:
        if (("=" in i) and (len(i) >= 3) and (not i[i.index("=") - 1] == r"\\")
                and
            (i.count("=")
             == 1)):  # a bit messy but probably the best way to do this
            split: List[str] = i.split("=")
            kwargs[split[0]] = split[1]

        elif i.startswith(config.flag_prefix):
            flags.append(i[2:])
        else:
            args.append(i)

    await log("parsed arguments")
    return args, kwargs, flags
