import os
import sys
import importlib
from typing import List, Callable
from types import ModuleType

def load_middleware(directory: str) -> List[Callable]:
    """Function for creating the commands dict for the client."""

    resp: List[Callable] = []
    
    sys.path.append(directory)
    for i in os.listdir(directory):
        command: ModuleType = importlib.import_module(f'middleware.{i}.main')

        resp.append(command.run)
        
    return resp