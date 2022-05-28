from typing import Protocol, Dict, Type
from .object import Object

class Output(Protocol):
    async def print(self, *values: str) -> None:
        ...

    async def write(self, *values: str) -> None:
        ...

    async def success(self, *values: str) -> None:
        ...

    async def error(self, *values: str) -> None:
        ...

class EngineCallable(Protocol):
    def call(self, *args, **kwargs):
        ...