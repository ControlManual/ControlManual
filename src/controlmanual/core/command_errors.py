__all__ = (
    "CMException",
    "GenericError",
    "InternalError",
    "CannotCast",
    "ParseError",
    "Undefined",
    "CannotCall",
    "UnsupportedObject",
    "InvalidArgument"
)

class CMException(Exception):
    """Base class representing a Control Manual command error."""

    def __init__(self, *args, **kwargs):
        self._cm_context = {}
        super().__init__(*args, **kwargs)

class GenericError(CMException):
    """Generic command error."""

    pass

class InternalError(CMException):
    """Error occured internally."""

    pass

class CannotCast(CMException):
    """Failed to cast a type."""

    pass

class ParseError(CMException):
    """Failed to parse the input string."""

    pass

class Undefined(CMException):
    """Tried to access something that does not exist."""

    pass

class CannotCall(CMException):
    """Target object isn't callable."""

    pass

class UnsupportedObject(CMException):
    """Internal object doesn't derive from Object."""

    pass

class InvalidArgument(CMException):
    """An invalid argument was passed."""

    pass