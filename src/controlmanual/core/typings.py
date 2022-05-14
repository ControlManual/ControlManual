from typing import Protocol

class Output(Protocol):
    async def print(self, *values: str) -> None:
        ...

    async def write(self, *values: str) -> None:
        ...

    async def success(self, *values: str) -> None:
        ...

    async def error(self, *values: str) -> None:
        ...