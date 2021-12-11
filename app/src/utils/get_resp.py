import sys
import io
from typing import Callable

def get_resp(fn: Callable, *args, **kwargs) -> str:
    """No longer works."""
    
    old_stdout = sys.stdout
    sys.stdout = buffer = io.StringIO()

    fn(*args, **kwargs)

    sys.stdout = old_stdout
    return buffer.getvalue()