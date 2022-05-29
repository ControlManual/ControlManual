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
    Tuple
)
from typing_extensions import ParamSpec
from .command_errors import (
    Undefined,
    ParseError,
    CannotCast,
    UnsupportedObject,
    CannotCall,
    InvalidArgument
)
from inspect import isclass
from types import FunctionType, MethodType

T = TypeVar("T")
A = TypeVar("A", bound = "Object")
B = TypeVar("B")
P = ParamSpec("P")

__all__ = (
    "expose",
    "Object",
    "String",
    "Integer",
    "Objects"
)

def _make_int(data: Any) -> int:
    string = str(data)
    if string.isdigit():
        return int(string)
    
    obj = Objects.ensure_object(data)
    raise InvalidArgument(f'cannot cast "{obj}" to Integer')

def _make_bool(data: Any) -> bool:
    string = str(data)
    
    if string.isdigit():
        num = int(string)

        return num > 0

    return bool(string) if data in BUILTINS else True

BUILTINS: Tuple[type, type, type, type] = (str, float, bool, int)
CM_CASTS: Dict[type, Callable] = {
    str: str,
    int: _make_int,
    bool: _make_bool,
}

def map_type(
    data: Any,
    actual: Union["Object", type],
    typ: Type[T]
) -> T:
    mappable_reversed = {v: k for k, v in CM_MAPPABLE.items()}

    if actual is not typ:
        if (actual in CM_MAPPABLE) or (actual in mappable_reversed):
            target = CM_MAPPABLE.get(actual) \
            if issubclass(
                actual,  # type: ignore
                BUILTINS
            ) \
            else mappable_reversed.get(actual)  # type: ignore

            if target is typ:
                assert target
                data = target(data)
    
    return data

class Object(ABC, Generic[T]):
    """Abstract class representing a Control Manual object."""
    type_name: str
    """Name of the type."""
    raw_data_type: Type[T]
    """Type of the raw data."""

    def __init__(self, raw_data: T):
        typ = self.raw_data_type
        actual: type = type(raw_data)

        if (actual is not typ) and ((actual in BUILTINS) and (typ in BUILTINS)):
            raw_data = CM_CASTS[typ](map_type(raw_data, actual, typ))

        self._raw_data = map_type(raw_data, type(raw_data), typ)

    def __init_subclass__(cls, raw_data_type: Type[T], type_name: Optional[str] = None) -> None:
        cls.type_name = type_name or cls.__name__
        cls.raw_data_type = raw_data_type

    def cast(self, target: "Object", datatype: A) -> A:
        """Cast a type to a different type."""
        raise CannotCast(f'cannot cast "{target.type_name}" object to "{datatype.type_name}"')

    @abstractmethod
    def to_string(self) -> str:
        """Convert the object to a string representation."""
        ...

    def to_debug_string(self) -> str:
        """Convert the object to a debug representation."""
        return self.__repr__()

    @property
    def raw_data(self) -> T:
        """Raw internal data."""
        return self._raw_data

    def __repr__(self) -> str:
        return self.to_string()

class Function(Generic[P, T], Object[Callable[P, T]], raw_data_type = FunctionType):
    def call(self, *args: P.args, **kwargs: P.kwargs) -> T:
        return self.raw_data(*args, **kwargs)  

    def to_string(self) -> str:
        return f'<internally defined function at {hex(id(self.raw_data))}>'

def expose(func: Callable[P, T]) -> Callable[P, T]:
    """Expose a function to Control Manual."""
    func.__cm_exposed = True
    return func

class String(Object[str], raw_data_type = str):
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
        return f"'{self._raw_data}'"

class Integer(Object[int], raw_data_type = int):
    pass

class Boolean(Object[int], raw_data_type = bool):
    def to_string(self):
        return str(self.raw_data).lower()

class Float(Object[float], raw_data_type = float):
    pass

Variable = Union[Type[Object], Object]
CM_MAPPABLE: Dict[type, Type[Object]] = {
    str: String,
    int: Integer,
    FunctionType: Function,
    MethodType: Function,
    bool: Boolean,
    float: Float
}
BASE_OBJECTS = {
    'String': String,
    'Integer': Integer,
    'Boolean': Boolean,
    'Function': Function,
    'Float': Float,
    'true': Boolean(True),
    'false': Boolean(False),
}

ObjectLike = Union[Variable, str, int, FunctionType, MethodType]

class Objects:
    """Class representing all objects in the current session."""

    def __init__(
        self,
        objects: Optional[Dict[str, Variable]] = None,
    ) -> None:
        self._objects = objects or BASE_OBJECTS
    
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