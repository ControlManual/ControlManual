from abc import ABC, abstractmethod
from typing import Callable, Generic, TypeVar
from typing_extensions import ParamSpec

T = TypeVar("T")
P = ParamSpec("P")

class Command(ABC):
    """Abstract class representing a command."""
