from abc import ABC, abstractmethod
from typing import TypeVar, Generic, Callable, Optional
from typing_extensions import ParamSpec
from .command_errors import CannotCast

T = TypeVar("T")
A = TypeVar("A", bound = "Object")
P = ParamSpec("P")

__all__ = (
    "ExposedFunction",
    "expose",
    "Object"
)

class ExposedFunction(Generic[P, T]):
    """Object containing an object function exposed to the Control Manual engine."""
    def __init__(self, func: Callable[P, T]):
        self._func = func

    @property
    def func(self) -> Callable[P, T]:
        """Function exposed to the Control Manual engine."""
        return self._func

    def __call__(self, *args: P.args, **kwargs: P.kwargs) -> T:
        return self.func(*args, **kwargs)

def expose(func: Callable[P, T]) -> ExposedFunction[P, T]:
    """Expose a function to Control Manual."""
    return ExposedFunction(func)

class Object(ABC, Generic[T]):
    """Abstract class representing a Control Manual object."""

    type_name: str
    """Name of the type."""

    def __init__(self, raw_data: T):
        self._raw_data = raw_data

    def __init_subclass__(cls, type_name: Optional[str] = None) -> None:
        cls.type_name = type_name or cls.__name__

    @abstractmethod
    def cast(self, value: "Object", target: A) -> A:
        """Cast a type to a different type."""
        raise CannotCast(f"cannot cast {self.type_name} to {self.type_name}")

    @abstractmethod
    def to_string(self) -> str:
        """Convert the object to a string representation."""
        ...

    def to_debug_string(self) -> str:
        """Convert the object to a debug representation."""
        return f"{self.type_name} object"

    @property
    def raw_data(self) -> T:
        """Raw internal data."""
        return self._raw_data

    def accessed(self) -> None:
        """Called when the object is accessed."""
        return

class String(Object[str]):
    def to_string(self):
        return self._raw_data
    
    @expose
    def upper(self):
        """Convert the string to upper case."""
        return self._raw_data.upper()

    @expose
    def lower(self):
        """Convert the string to lower case."""
        return self._raw_data.lower()

