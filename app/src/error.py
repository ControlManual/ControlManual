import sys
import traceback
from .config import Config
from . import utils # for some reason using "from .utils import *" was literally just not defining anything

class CMError:
    """Control Manual Error."""
    def __init__(self, message: str, error: Exception = None, kill: bool = False) -> None:
        """Control Manual Error."""
        self._message = message
        self._error = error
        self._kill = kill
        self._config = Config()
    
    @property
    def config(self) -> Config:
        """Config object."""
        return self._config

    @property
    def message(self) -> str:
        """Message of the exception."""
        return self._message
    
    @property
    def error(self) -> Exception:
        """Exception object."""
        return self._error
    
    @property
    def kill(self) -> bool:
        """Whether to kill the program after handling the exception."""
        return self._kill
    
    def print_exc(self) -> None:
        """Print out the error message."""
        msg = self.get_exc()
        print(msg)
    
    def get_exc(self) -> str:
        """Function for getting the error message string."""
        
        if self._kill:
            status: str = f'{utils.red}FATAL ERROR'
        else:
            status: str = f'{utils.red}ERROR'

        trace: str = ''.join(traceback.format_exception(None, self.error, self.error.__traceback__)) if self._error else 'No Traceback.'
        resp: str = f'\n{status}: {self.message}\n{utils.red}TRACEBACK: {utils.bright}\n{trace}{utils.reset}'

        return resp
    def raise_exc(self) -> None:
        """Raise an exception."""
        self.print_exc()
        if self.kill:
            sys.exit(1)
        
