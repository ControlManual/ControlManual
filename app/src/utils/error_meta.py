from .codes import codes
from .make_meta import make_meta

def error_meta(code: int, trigger: str = 'generic'):
    """Function for generating error metadata."""
    return make_meta(False, code, trigger, error = codes[code])