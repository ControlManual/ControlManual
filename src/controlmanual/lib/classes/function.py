from pydantic import BaseModel
from typing import List

class Function(BaseModel):
    """Object representing a command function."""
    arguments: List[str]
    """List showing the arguments that the function accepts."""
    script: List[str]
    """List containing all lines in the function."""
    defined: bool
    """Whether the function has finished being executed."""