class CMBaseException(Exception):
    """Base class for command errors."""
    pass

class InvalidArguments(CMBaseException):
    """Raised when arguments passed to the command are invalid."""

    pass


class NotEnoughArguments(CMBaseException):
    """Raised when insufficent arguments are passed to the command."""

    pass


class Other(CMBaseException):
    """Raised as a custom error."""

    pass


class Exists(CMBaseException):
    """Raised when something exists, that shouldn't."""

    pass


class NotExists(CMBaseException):
    """Raised when something doesn't exist, that should."""

    pass


class InvalidArgument(CMBaseException):
    """Raised when a specific argument is invalid."""

    pass


class APIError(CMBaseException):
    """Raised when an error occurs related to the API."""

    pass


class NothingChanged(CMBaseException):
    """Raised when nothing changes on a command execution."""

    pass


class Collision(CMBaseException):
    """Raised when a collision between items occurs."""

    pass

class NotAnIterator(CMBaseException):
    """Raised when trying to iterate over a command that doesn't support iteration."""
    pass