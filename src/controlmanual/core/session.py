from .typings import Output
from .command_errors import CMException
from ._parse import parse

class Session:
    """Main class representing a Control Manual session."""

    def __init__(self, output: Output):
        self._output = output
        self._variables = {}
        self._commands = {}
        self._ready = False

    async def execute(self, command: str):
        """Execute a command."""
        try:
            parsed = parse(command)
        except CMException as e:
            await self.handle_cm_exc(e)

    async def handle_cm_exc(self, err: CMException):
        await self._output.error(str(err))