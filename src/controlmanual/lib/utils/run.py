import asyncio
from typing import Awaitable, TypeVar

T = TypeVar("T")


def run(coro: Awaitable[T]) -> T:
    """Run a coroutine synchronously."""
    try:
        res = asyncio.run(coro)
    except BaseException:
        loop = asyncio.get_event_loop()
        res = loop.run_until_complete(coro)

    return res
