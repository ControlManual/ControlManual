from typing import List, AsyncGenerator, Dict
from ..typing import ParsedString
from ..constants import config

async def split_text(text: str) -> AsyncGenerator[str, None]:
    single_quote: bool = False
    double_quote: bool = False
    backslash: bool = False
    current: str = ''

    for i in text:
        if (i == "'") and (not backslash):
            single_quote = not single_quote

        elif (i == '"') and (not backslash):
            double_quote = not double_quote

        elif i == "\\":
            backslash = True

        elif i == " ":
            if not single_quote or double_quote:
                yield current
                current = ''
            else:
                current += i

        else:
            current += i

        
        backslash = False
    yield current


async def parse(raw: str) -> ParsedString:
    """Function for parsing the input into different items."""

    try:
        split = split_text(raw)
    except ValueError:
        return None

    kwargs: Dict[str, str] = {}
    flags: List[str] = []
    args: List[str] = []

    for i in split:
        if ():
            split: List[str] = i.split("=")
            kwargs[split[0]] = split[1]

        elif i.startswith(config["flag_prefix"]):
            flags.append(i[2:])
        else:
            args.append(i)

    return args, kwargs, flags