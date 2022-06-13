from dataclasses import dataclass
from typing import List, Dict, AsyncIterator
from .object import Object, Objects

__all__ = ("parse", "ParsedString")


@dataclass
class ParsedString:
    """Class representing a parsed string."""

    command: str
    raw: str
    pos_args: List[Object]
    keyword_args: Dict[str, Object]
    opens_context: bool


async def parse(data: str, objects: Objects) -> AsyncIterator[ParsedString]:
    """Parse a string of data."""
    ...
    # for i in re.split("&&| && ", data):
    # yield await _handle_parse(i, objects)
