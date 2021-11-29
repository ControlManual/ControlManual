from .codes import codes
from .make_meta import make_meta

def error_meta(code: int):
    """Function for generating error metadata."""
    return make_meta(False, code, error = codes[code])