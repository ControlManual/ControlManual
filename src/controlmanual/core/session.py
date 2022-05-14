from .typings import Output

class Session:
    """Main class representing a Control Manual session."""

    def __init__(self, output: Output):
        self._output = output
        self._variables = {}

    async def execute(self, command: str):
        """Execute a command."""
        pass