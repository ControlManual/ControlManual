from abc import ABC, abstractmethod
from typing import (
    TypeVar,
    Generic,
    Callable,
    Optional,
    Type,
    Dict,
    overload,
    Union,
    Callable,
    Literal,
    Any,
    List,
)
from typing_extensions import ParamSpec, TypeGuard
from .command_errors import (
    Undefined,
    ParseError,
    CannotCast,
    UnsupportedObject,
    CannotCall
)
from inspect import isclass
from types import FunctionType, MethodType

T = TypeVar("T")
A = TypeVar("A", bound = "Object")
P = ParamSpec("P")

__all__ = (
    "expose",
    "Object",
    "String",
    "Integer",
    "Objects"
)

class Object(ABC, Generic[T]):
    """Abstract class representing a Control Manual object."""

    type_name: str
    """Name of the type."""

    def __init__(self, raw_data: T):
        self._raw_data = raw_data

    def __init_subclass__(cls, type_name: Optional[str] = None) -> None:
        cls.type_name = type_name or cls.__name__

    def cast(self, target: "Object", datatype: A) -> A:
        """Cast a type to a different type."""
        raise CannotCast(f'cannot cast "{target.type_name}" object to "{datatype.type_name}"')

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

    def __repr__(self) -> str:
        return self.to_string()

class Function(Generic[P, T], Object[Callable[P, T]]):
    def call(self, *args: P.args, **kwargs: P.kwargs) -> T:
        return self.raw_data(*args, **kwargs)

    def to_string(self) -> str:
        return f'Function(<internally defined at {hex(id(self.raw_data))}>)'


def expose(func: Callable[P, T]) -> Callable[P, T]:
    """Expose a function to Control Manual."""
    func.__cm_exposed = True
    return func

class String(Object[str]):
    def to_string(self):
        return self._raw_data
    
    def to_debug_string(self) -> str:
        return f'"{self._raw_data}"'

    @expose
    def upper(self):
        """Convert the string to upper case."""
        return self._raw_data.upper()

    @expose
    def lower(self):
        """Convert the string to lower case."""
        return self._raw_data.lower()

    def cast(self, target: Object, datatype: Type[A]) -> A:
        return datatype(target.to_string())

    def __repr__(self) -> str:
        return f"String('{self._raw_data}')"

class Integer(Object[int]):
    def to_string(self):
        return self._raw_data

    def cast(self, target: Object, datatype: Type[A]) -> A:
        return datatype(target.to_string())

    def __repr__(self) -> str:
        return f"Integer({self._raw_data})"

Variable = Union[Type[Object], Object]
CM_MAPPABLE: Dict[type, Type[Object]] = {
    str: String,
    int: Integer,
    FunctionType: Function,
    MethodType: Function
}
ObjectLike = Union[Variable, str, int, FunctionType, MethodType]

class Objects:
    """Class representing all objects in the current session."""

    def __init__(
        self,
        objects: Dict[str, Variable],
    ) -> None:
        self._objects = objects
    
    @property
    def objects(self):
        return self._objects

    @overload
    def lookup(
        self,
        name: str,
        ensure_callable: bool = False,
        ensure_instance: Literal[False] = False,
        ensure_not_instance: Literal[True] = True
    ) -> Type[Object]:
        ...

    @overload
    def lookup(
        self,
        name: str,
        ensure_callable: bool = False,
        ensure_instance: Literal[True] = True,
        ensure_not_instance: Literal[False] = False
    ) -> Object:
        ...

    def lookup(
        self,
        name: str,
        ensure_callable: bool = False,
        ensure_instance: bool = False,
        ensure_not_instance: bool = False
    ) -> Variable:
        obj = self._objects.get(name) if '.' not in name else self._lookup_attribute(name)

        if not obj:
            raise Undefined(f'"{name}" is not defined')

        if ensure_instance:
            if isclass(obj):
                raise Exception

        if ensure_not_instance:
            if not isclass(obj):
                raise Exception

        if ensure_callable:
            if not hasattr(obj, 'call'):
                raise Exception

        return obj  # type: ignore

    def _parse_string(self, string: str) -> List[str]:
        return (string if not string.startswith('.') else string[1:]).split('.')

    def _lookup_attribute(self, string: str) -> Variable:
        split = self._parse_string(string)

        if len(split) == 1:
            raise ParseError('unexpected "."')

        obj = self.lookup(split.pop(0))

        return self.lookup_object_attribute(obj, '.'.join(split))

    @staticmethod
    def ensure_object(obj: Any) -> Object:
        if not ((isclass(obj) and issubclass(obj, Object)) or (isinstance(obj, Object))):
            obj_type = obj if isclass(obj) else type(obj)
            mapped_type = CM_MAPPABLE.get(obj_type)

            if mapped_type:
                return mapped_type(obj)

            raise UnsupportedObject(f'"{obj_type.__name__}" does not derive from Object')

        return obj  # type: ignore

    @classmethod
    def ensure_callable(cls, raw_obj: Any) -> Function:
        obj = cls.ensure_object(raw_obj)

        if not hasattr(obj, 'call'):
            raise CannotCall(f'"{obj.type_name}" object is not callable')
        
        return obj  # type: ignore

    def lookup_object_attribute(self, obj: ObjectLike, string: str) -> Variable:
        split = self._parse_string(string)
        attr_name = split.pop(0)
        self.ensure_object(obj)

        raw_attr = getattr(obj, attr_name, None)

        if (not raw_attr) or (not getattr(raw_attr, '__cm_exposed', None)):
            raise Undefined(f'"{obj.type_name}" object has no attribute "{attr_name}"')  # type: ignore

        attr = raw_attr if not split else self.lookup_object_attribute(raw_attr, '.'.join(split))
        return self.ensure_object(attr)