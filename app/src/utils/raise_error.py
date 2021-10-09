from ..error import CMError

def raise_error(message: str, error: Exception = None, kill: bool = False) -> None:
        """Function for raising an error."""
        CMError(message, error, kill).raise_exc()