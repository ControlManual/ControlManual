from .config import config
from typing import TYPE_CHECKING, List, Optional, Any, Dict
import logging
from .parser import parse
from .help import HelpCommand

if TYPE_CHECKING:
    from ..client import Client


class CommandHandler:
    def __init__(self, client: "Client") -> None:
        self._client = client

    @property
    def client(self) -> "Client":
        """Client class."""
        return self._client

    async def handle_function(self, cmd: str, raw: str):
        client = self.client
        errors = config["errors"]
        crfn: Optional[str] = client.current_function

        for i in range(2):
            is_syntax: bool = cmd == config["functions"][i]

            if is_syntax:
                if crfn:
                    if client._function_open:
                        if i:
                            client.error(errors["function_open"])
                        else:
                            client.functions[client.current_function]["defined"] = True
                            client._function_open = False
                            client._current_function = None
                    elif not i:
                        client.error(errors["function_not_open"])
                    else:
                        client._function_open = True
                else:
                    client.error(errors["function_undefined"])
                    return True

        if client._function_open:
            client._functions[crfn]["script"].append(raw)

            return True

    async def run_command(self, command: str):
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
    
        args, kwargs, flags = await parse(raw_args)
        cmd: str = cmd.lower()

        commands = client.commands

        if await self.handle_function(cmd, command):
            return

        if cmd == config["help_command"]:
            hlp = HelpCommand(commands, args)
            target = hlp.print_help if args else hlp.print_argument_help if len(args) > 1 else hlp.print_command_help

            return await target()

        elif cmd in commands:
            await self.execute(cmd, raw_args, args, kwargs, flags)
        else:
            logging.info("command not found")
            client.error(errors["unknown_command"])
    
    async def run_string(self, text: str) -> Any:
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

        for comm in cmds:
            await self.run_command(comm)

    async def execute(self, cmd: str, raw_args: str, args: List[str], kwargs: Dict[str, str], flags: List[str]):
        """Function for executing a command."""
        errors = config["errors"]
        client = self.client
        current_command = client.commands[cmd]

        if "exe" in current_command:
            raise NotImplementedError('binary execution not implemented')

        runner = current_command["entry"]

        try:
            await runner(raw_args, args, kwargs, flags, client)
        except Exception as e:
            emap = client.error_map

            if type(e) in emap:
                return client.error(str(e))

            logging.error(f"command ran into exception: {e}")
            if isinstance(e, PermissionError):
                logging.debug("permission error found")
                client.error(errors["permission_error"])
            else:
                failure: str = errors["command_error"].replace("{cmd}", cmd)
                client.error(failure)
                await client.show_exc(e)