from __future__ import annotations
from pydantic import BaseModel
from typing import Optional, Dict, Literal, TYPE_CHECKING

if TYPE_CHECKING:
    from ..typings import CommandCallable, CommandIterator

class Command(BaseModel):
    """Object representing a command."""
    help: Optional[str]
    """Help string of the command."""
    usage: Optional[str]
    """Usage string of the command."""
    package: Optional[str]
    """Package name of the command."""
    warning: str
    """Usage warning of the command."""
    args: Optional[Dict[str, str]]
    """Argument dictionary of the command."""
    flags: Optional[Dict[str, str]]
    """Flag dictionary of the command."""
    args_help: Optional[dict]
    """Argument help dictionary."""
    path: str
    """Location of the command."""
    entry: CommandCallable
    """Entry point for the command."""
    iterator: Optional[CommandIterator]
    """Iterator function of the command."""
    is_binary: Literal[False]
    """Whether the command is a binary."""
