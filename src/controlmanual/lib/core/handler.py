import logging
from typing import TYPE_CHECKING, Dict, List, Optional, AsyncIterator
from ..typings import CommandResponse, Function

from .config import config
from .help import HelpCommand
from .parser import parse
from ..constants import CMBaseException

if TYPE_CHECKING:
    from lib.client import Client


class CommandHandler:
    def __init__(self, client: "Client") -> None:
        self._client = client

    @property
    def client(self) -> "Client":
        """Client class."""
        return self._client

    async def handle_function(self, cmd: str, raw: str) -> bool:
        """Handle function parsing."""
        client = self.client
        errors = config["errors"]
        current: Optional[str] = client.current_function
        is_syntax: bool = cmd in config["functions"]

        if not current:
            if is_syntax:
                client.error(errors["function_undefined"])
                return True
            return False

        target: Function = client.functions[current]

        if is_syntax:
            bool_map: Dict[str, bool] = {
                value: not index
                for index, value in enumerate(config["functions"])
            }

            if client.function_open == bool_map[cmd]:
                client.error(errors[f"function_{'' if cmd == '{' else 'not_'}open"])
                return True
            
            client.function_open = not client.function_open

            if not client.function_open:
                target["defined"] = True
                client.current_function = None
        else:
            logging.debug("appended to target")
            target["script"].append(raw)
        
        return True  
    

    async def run_command(self, command: str) -> CommandResponse:
        errors = config["errors"]
        client = self.client

        command = await client.load_variables(command)
        while True:
            if command not in [str(i) for i in range(1, 11)]:
                break

            try:
                command = client.cmd_history[int(command) - 1]
            except IndexError:
                return logging.error("index error with cmd history")

        split: List[str] = command.split(" ")

        if not any(split):
            return

        while not split[0]:
            split.pop(0)

        raw_args = " ".join(split[1:])  # unsplit string of arguments
        cmd = split[0]

        while True:
            if cmd not in client.aliases:
                break

            logging.info("alias found")
            spl = client.aliases[cmd].split(" ")
            cmd = spl[0]
            if len(spl) > 1:
                excess: str = " ".join(spl[1:])
                raw_args = excess + " " + raw_args

        raw_args = await client.load_functions(raw_args)

        args, kwargs, flags = await parse(raw_args)
        cmd: str = cmd.lower()

        commands = client.commands

        if await self.handle_function(cmd, command):
            return

        if cmd == config["help_command"]:
            hlp = HelpCommand(commands, args, self.client)
            target = (
                hlp.print_help
                if args
                else hlp.print_argument_help
                if len(args) > 1
                else hlp.print_command_help
            )

            return await target()

        elif cmd in commands:
            return await self.execute(cmd, raw_args, args, kwargs, flags)
        else:
            logging.info("command not found")
            client.error(errors["unknown_command"])

    async def run_string(self, text: str) -> CommandResponse:
        """Function for running a command."""
        client = self.client
        logging.info(f"handling string: {text}")

        if text not in [str(i) for i in range(1, 11)]:
            client.cmd_history.append(text)

        for i in config["comments"]:
            if text.startswith(i):
                logging.info("comment found, ending")
                return

        cmds = text.split(config["seperator"])
        res = None # in case cmds is empty

        for comm in cmds:
            res: CommandResponse = await self.run_command(comm)

        return res

    async def execute(
        self,
        cmd: str,
        raw_args: str,
        args: List[str],
        kwargs: Dict[str, str],
        flags: List[str],
    ) -> CommandResponse:
        """Function for executing a command."""
        errors = config["errors"]
        client = self.client
        current_command = client.commands[cmd]

        if "exe" in current_command:
            raise NotImplementedError("binary execution not implemented")

        runner = current_command["entry"]

        if 'iter' in flags:
            runner = current_command["iterator"]

        try:
            if not runner:
                return None

            coro = runner(raw_args, args, kwargs, flags, client)

            if isinstance(coro, AsyncIterator):
                return coro

            return await coro
        except CMBaseException as e:
            client.error(str(e))
    
        except Exception as e:
            logging.error(f"command ran into exception: {e}")
            if isinstance(e, PermissionError):
                logging.debug("permission error found")
                client.error(errors["permission_error"])
            else:
                failure: str = errors["command_error"].replace("{cmd}", cmd)
                client.error(failure)
                await client.show_exc(e)
