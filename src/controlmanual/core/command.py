from typing import Callable, Generic, TypeVar, Awaitable
from typing_extensions import ParamSpec
from .typings import CommandSchema

T = TypeVar("T")
P = ParamSpec("P")


class Command(Generic[P, T]):
    """Class representing a command."""

    def __init__(
        self,
        caller: Callable[P, Awaitable[T]],
        description: str,
        schema: CommandSchema,
    ) -> None:
        self._caller = caller
        self._description = description
        self._schema = schema

    async def __call__(self, *args: P.args, **kwargs: P.kwargs) -> T:
        ...
