from dataclasses import dataclass
from typing import List, Dict

@dataclass
class ParsedString:
    command: str
    raw: str
    pos_args: List[str]
    keyword_args: Dict[str, str]
    flags: Dict[str, str]

def parse(data: str):
    """Parse a string of data."""
    pass