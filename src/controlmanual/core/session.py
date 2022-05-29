from .typings import Output
from .command_errors import CMException
from ._parse import parse
from .object import Objects

class Session:
    """Main class representing a Control Manual session."""

    def __init__(self, output: Output):
        self._output = output
        self._objects = Objects()
        self._commands = {}
        self._ready = False

    async def execute(self, command: str):
        """Execute a command."""
        try:
            for parsed in parse(command, self._objects):
                print(parsed)
        except CMException as e:
            await self.handle_cm_exc(e)

    async def handle_cm_exc(self, err: CMException):
        await self._output.error(str(err))