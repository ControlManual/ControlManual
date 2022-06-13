from abc import ABC, abstractmethod
from typing import (
    TypeVar,
    Generic,
    Callable,
    Optional,
    Type,
    Dict,
    Callable,
    NoReturn,
)
from typing_extensions import ParamSpec
from .command_errors import (
    Undefined,
    CannotCall,
)

T = TypeVar("T")
P = ParamSpec("P")


class BaseObject(ABC, Generic[T]):
    """Abstract base class representing a Control Manual object."""

    name: str
    """Name of the type."""

    def __init_subclass__(cls, name: Optional[str] = None) -> None:
        cls.name = name or cls.__name__
        return super().__init_subclass__()

    @abstractmethod
    async def cm_init(self, *args, **kwargs) -> None:
        """Called when the engine initalizes a new instance of the object."""
        ...

    @abstractmethod
    def cm_get_attribute(self, name: str) -> "BaseObject":
        """Get an attribute from the engine."""
        ...

    @abstractmethod
    def cm_set_attribute(self, name: str, value: "BaseObject") -> None:
        """Set an attribute on the object from the engine."""
        ...

    @abstractmethod
    async def cm_call(self, *args, **kwargs) -> "BaseObject":
        """Call the object from the engine."""
        ...

    @abstractmethod
    def to_string(self) -> str:
        """Convert the object to a string representation."""
        ...


class Object(BaseObject, ABC):
    """High level base class for objects."""

    def __init__(self):
        self._attributes: Dict[str, BaseObject] = {}

    @abstractmethod
    async def cm_init(self, *args, **kwargs) -> None:
        ...

    def cm_get_attribute(self, name: str) -> BaseObject:
        obj = self._attributes.get(name)

        if not obj:
            raise Undefined(f'"{self.name}" object has no attribute "{name}"')

        return obj

    def cm_set_attribute(self, name: str, value: BaseObject) -> None:
        self._attributes[name] = value

    async def cm_call(self) -> NoReturn:
        raise CannotCall(f'"{self.name}" object is not callable')

    @abstractmethod
    def to_string(self) -> str:
        ...


def expose(func: Callable[P, T]) -> Callable[P, T]:
    """Expose a function to Control Manual."""
    func.__cm_exposed = True
    return func


class DataType(Object, ABC, Generic[T]):
    """Abstract class for mapping Python types to Control Manual objects."""

    data_type: Type[T]
    """Type of the data."""

    @property
    def data(self) -> T:
        """Internal raw data."""
        return self._data

    def cm_init(self, data: T) -> None:
        self._data = data

    def to_string(self) -> str:
        return str(self._data)

    def __init_subclass__(
        cls, data_type: Type[T], name: Optional[str] = None
    ) -> None:
        cls.data_type = data_type
        return super().__init_subclass__(name)


class String(DataType[str], data_type=str):
    @expose
    def upper(self):
        return self.data.upper()

    @expose
    def lower(self):
        return self.data.lower()


class Integer(DataType[int], data_type=int):
    pass


class Boolean(DataType[bool], data_type=bool):
    pass


class Objects:
    """Class acting as a handler for objects."""

    def __init__(self):
        self._objects: Dict[str, Object] = {}

    @property
    def raw(self) -> Dict[str, Object]:
        return self._objects

    async def lookup(self, name: str) -> Object:
        """Lookup an object."""
        ...

    async def delete(self, name: str) -> None:
        """Delete an existing object."""
        ...

    async def new(self, name: str, value: Object) -> None:
        """Create a new object."""
        ...

    async def exists(self, name: str) -> bool:
        """Check if an object exists under the specified name."""
        ...
