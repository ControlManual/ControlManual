import sys
import io

def get_resp(command, *args, **kwargs) -> str:
    """Function for getting the printed response of an output as a string."""
    old_stdout = sys.stdout
    sys.stdout = buffer = io.StringIO()

    command(*args, **kwargs)

    sys.stdout = old_stdout
    return buffer.getvalue()