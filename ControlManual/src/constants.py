import os

cm_dir: str = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

# Errors
class InvalidArguments(Exception):
    """Raised when arguments passed to the command are invalid."""
    pass

class NotEnoughArguments(Exception):
    """Raised when insufficent arguments are passed to the command."""
    pass

class Other(Exception):
    """Raised as a custom error."""
    pass

class Exists(Exception):
    """Raised when something exists, that shouldn't."""
    pass

class NotExists(Exception):
    """Raised when something doesn't exist, that should."""
    pass

class InvalidArgument(Exception):
    """Raised when a specific argument is invalid."""
    pass

class APIError(Exception):
    """Raised when an error occurs related to the API."""
    pass

class NothingChanged(Exception):
    """Raised when nothing changes on a command execution."""
    pass

class Collision(Exception):
    """Raised when a collision between items occurs."""
    pass


# Info
__title__ = "ControlManual"
__author__ = "ZeroIntensity and 2231puppy"
__license__ = "MIT"
__version__ = "0.0.4.0"

