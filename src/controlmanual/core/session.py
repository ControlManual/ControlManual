from .typings import UI
from .command_errors import CMException
from ._parse import parse
from .object import Objects
from typing import Dict
from .command import Command


class Session:
    """Main class representing a Control Manual session."""

    def __init__(
        self,
        ui: UI,
        config_dir: str,
        logs_dir: str,
        config_file: str,
        commands_dir: str,
    ) -> None:
        self._ui = ui
        self._objects = Objects()
        self._commands: Dict[str, Command] = {}
        self._ready = False
        self._config_dir = config_dir
        self._logs_dir = logs_dir
        self._config_file = config_file
        self._commands_dir = commands_dir

    async def execute(self, command: str):
        """Execute a command."""
        try:
            async for parsed in await parse(command, self._objects):
                print(parsed)
        except CMException as e:
            await self.handle_cm_exc(e)

    async def handle_cm_exc(self, err: CMException):
        await self._ui.error(str(err))

    async def load_commands(self):
        """Load all commands."""
        ...
