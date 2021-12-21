import os
import sys
import importlib
from typing import List, Coroutine
from types import ModuleType
from ..logger import log


async def load_middleware(directory: str) -> List[Coroutine]:
    """Function for creating the commands dict for the client."""
    await log("loading middleware")

    resp: List[Coroutine] = []

    sys.path.append(directory)
    for i in os.listdir(directory):
        command: ModuleType = importlib.import_module(f"middleware.{i}.main")

        resp.append(command.run)

    return resp
