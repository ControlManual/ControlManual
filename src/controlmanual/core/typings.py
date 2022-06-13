from typing import (
    Protocol,
    Awaitable,
    TypeVar,
    Callable,
    Union,
    runtime_checkable,
    Dict,
    TypedDict,
    Generic,
    Type,
    TYPE_CHECKING,
)
from typing_extensions import ParamSpec

if TYPE_CHECKING:
    from .object import Object

T = TypeVar("T", covariant=True)
A = TypeVar("A", bound="Object")
P = ParamSpec("P")


class UI(Protocol):
    async def print(self, *values: str) -> None:
        ...

    async def write(self, *values: str) -> None:
        ...

    async def success(self, *values: str) -> None:
        ...

    async def error(self, *values: str) -> None:
        ...

    async def input(self) -> str:
        ...


class CommandArgument(TypedDict, Generic[A]):
    required: bool
    description: str
    type: Type[A]


@runtime_checkable
class EngineCallable(Protocol[P, T]):
    def cm_call(self, *args: P.args, **kwargs: P.kwargs) -> T:
        ...


MaybeAwaitable = Union[Awaitable[T], T]
MaybeCoroutine = Callable[P, MaybeAwaitable[T]]
CommandSchema = Dict[str, CommandArgument[A]]
