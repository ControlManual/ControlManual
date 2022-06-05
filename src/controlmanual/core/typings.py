from typing import (
    Protocol,
    Awaitable,
    TypeVar,
    Callable,
    Union,
    runtime_checkable,
)
from typing_extensions import ParamSpec

T = TypeVar("T", covariant=True)
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


@runtime_checkable
class EngineCallable(Protocol[P, T]):
    def cm_call(self, *args: P.args, **kwargs: P.kwargs) -> T:
        ...


MaybeAwaitable = Union[Awaitable[T], T]
MaybeCoroutine = Callable[P, MaybeAwaitable[T]]
