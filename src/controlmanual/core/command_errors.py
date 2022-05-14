__all__ = (
    "CMException",
    "GenericError",
    "InternalError",
    "CannotCast"
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