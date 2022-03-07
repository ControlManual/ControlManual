import re
from typing import Dict, List
import logging
from ..typings import ParsedString
from .config import config

__all__ = ["parse"]


def split_text(text: str) -> List[str]:
    if not text:
        return []

    res: List[str] = ['']
    index: int = 0
    quote: bool = False

    for i in text:
        if i == ' ':
            if not quote:
                index += 1
                res.append('')
                continue

        if i == '"':
            quote = not quote
            continue

        res[index] += i

    return res


async def parse(raw: str) -> ParsedString:
    """Function for parsing the input into different items."""
    kwargs: Dict[str, str] = {}
    flags: List[str] = []
    args: List[str] = []

    txt: str = re.split('|'.join(config['comments']), raw, maxsplit = 1)[0] # this gets rid of inline comments

    for i in split_text(txt):
        if re.match(
            ".+=.+", i
        ):  # im bad at regex so this might not be the best way to do it
            temp: list = i.split("=", maxsplit=1)
            kwargs[temp[0]] = temp[1]
        elif i.startswith(config["flag_prefix"]):
            flags.append(i[2:])
        else:
            args.append(i)
            

    logging.debug(f"a-k-f: {args, kwargs, flags}  raw: {raw}")
    return args, kwargs, flags
