import socket
from socket import AF_INET, SOCK_DGRAM, SOCK_STREAM
from typing import Any, Callable, Coroutine, Union, List

import psutil
from rich import box
from rich.align import Align
from rich.layout import Layout
from rich.panel import Panel
from rich.table import Table
from textual.reactive import Reactive
from textual.widget import Widget
import logging
from ..config import config
from rich.console import Console, ConsoleOptions
from contextlib import suppress
from textual.events import Key
from textual.keys import Keys
from rich.markup import escape

from ...client import Client

AD = "-"
AF_INET6 = getattr(socket, "AF_INET6", object())
proto_map = {
    (AF_INET, SOCK_STREAM): "tcp",
    (AF_INET6, SOCK_STREAM): "tcp6",
    (AF_INET, SOCK_DGRAM): "udp",
    (AF_INET6, SOCK_DGRAM): "udp6",
}

Callback = Callable[[str], Coroutine[None, None, None]]

__all__ = ["Console"]

def insert(base: str, index: int, value: str) -> str:
    """Insert a string to an index."""
    lis = list(base)
    lis.insert(index, value)
    return "".join(lis)


def remove(base: str, index: int) -> str:
    """Remove a character at an index."""
    lis = list(base)
    lis.pop(index)
    return "".join(lis)


def lower(x: str) -> str: return x.lower() if config["lowercase"] else x

class ConsolePanel:
    def __init__(self, client: Client, text: str, feed_text: str) -> None:
        self._client = client
        self._text = text
        self._feed_text = feed_text

    def __rich_console__(self, console: Console, options: ConsoleOptions):
        spl = self._feed_text.split('\n')
        height: int = options.max_height - 3

        txt = self._feed_text if height > len(spl) else '\n'.join(spl[-height:])

        yield Panel(
                f"{self._client.path} [success]{self._client.config['input_sep']}[/success] "
                + self._text
                + "\n"
                + txt,
                title="Terminal",
        )
class ConsoleClient:
    """Class for handling communication with the console widget."""

    def __init__(self, console: "Console") -> None:
        self._console = console

    def error(self, *args: Any) -> None:
        """Print an error message."""
        self.print(f"[error]Error[/error]: {' '.join(args)}")

    def success(self, *args: Any) -> None:
        """Print a success message."""
        self.print(f"[success]Success[/success]: {' '.join(args)}")

    def print(self, *args: Any) -> None:
        """Print a message."""
        self.write(*args, "\n")

    def write(self, *args: Any) -> None:
        """Write a message to the screen."""
        logging.debug(f"write called with args: {args}")
        for i in args:
            self._console.feed_text += str(i)

    def _k_v(self, key: Any, value: Any) -> None:
        self.print(f"[important]{key}[/important] - [success]{value}[/success]")

    def key_value(self, key: Any, value: Any = None):
        """Print a key value pair."""
        if isinstance(key, dict):
            for k, v in key.items():
                self._k_v(k, v)

        self._k_v(key, value)

    def empty(self):
        """Clear the console."""
        self._console.feed_text = ""


class Console(Widget):
    """Widget for representing the console interface."""

    callback: Callback
    title: str
    feed_text = Reactive("")
    client: Client
    autocomplete_index = Reactive(0)
    input_text = Reactive("")
    is_white = Reactive(False)
    cursor_index = Reactive(0)
    autosuggest: Reactive[List[str]] = Reactive([])

    def __init__(self, *args: Any, **kwargs: Any) -> None:
        self.client = kwargs.pop("client")
        self._console_client = ConsoleClient(self)
        self.callback = self.client.run_command  # type: ignore

        super().__init__(*args, **kwargs)

    def render(self) -> Union[Layout, ConsolePanel]:
        table = Table(box=box.SIMPLE)

        for x in ("Protocol", "Local Address", "Remote Address", "Status", "PID"):
            table.add_column(lower(x))

        proc_names = {p.pid: p.name for p in psutil.process_iter(["pid", "name"])}

        def imp(x):
            return f"[important]{x}[/important]"
        with suppress(psutil.AccessDenied):
            for conn in psutil.net_connections():
                name = proc_names.get(conn.pid, "?") or ""
                laddr = imp("%s:%s") % (conn.laddr)
                raddr = imp("%s:%s") % (conn.raddr) if conn.raddr else ""
                table.add_row(
                    imp(proto_map[(conn.family, conn.type)]),
                    laddr,
                    raddr or imp(AD),
                    imp(conn.status),
                    imp(name if isinstance(name, str) else name()),
                    imp(conn.pid),
                )

        inp_text = self.input_text
        find: List[str] = []

        if (' ' not in inp_text) and (inp_text):
            for command in list(self.client.commands.keys()) + list(self.client.aliases.keys()):
                if command.startswith(inp_text):
                    find.append(command)

        self.autosuggest = find

        text: str = self.make_text(self.input_text, self.is_white, self.cursor_index)
        autosuggest: str = ''

        for index, command in enumerate(find):
            color: str = '#919191' if not index == self.autocomplete_index else 'white'
            autosuggest += f"\n{' ' * (len(self.input_text + self.client.path_str) + 3)}[black on {color}]{command}[/black on {color}]"

        cons = ConsolePanel(self.client, text + autosuggest, self.feed_text)

        if not self.app.connections: # type: ignore
            return cons

        lay = Layout()

        lay.split_column(
            cons,
            Panel(Align.center(table), title="Connections"),
        )

        return lay

    @property
    def console_client(self):
        return self._console_client

    async def on_key(self, event: Key) -> None:
        key: str = event.key
        logging.debug(f"keypress: {key}")

        if key in [key.value for key in Keys]:
            if key == Keys.Enter:
                await self.callback(self.input_text)  # type: ignore
                self.input_text = ""
                self.cursor_index = 0

            if key == Keys.Left and self.cursor_index:
                self.cursor_index -= 1

            if key == Keys.Right and (self.cursor_index) != len(self.input_text):
                self.cursor_index += 1

            if key == Keys.ControlH and self.cursor_index:
                self.cursor_index -= 1
                self.autocomplete_index = 0
                self.input_text = remove(self.input_text, self.cursor_index)

            if key in {Keys.Down, Keys.Up}:
                tmp: int = self.autocomplete_index
                tmp += 1 if key == Keys.Down else -1

                if (tmp >= 0) and (tmp < len(self.autosuggest)):
                    self.autocomplete_index = tmp
            
            if key in {Keys.Tab, Keys.ControlI}:
                if self.autosuggest:
                    current: str = self.autosuggest[self.autocomplete_index]
                    temp: str = current[len(self.input_text):]
                    self.input_text += temp
                    self.cursor_index += len(temp)
                    self.autosuggest = []
                    self.autocomplete_index = 0

            return

        self.cursor_index += 1
        self.input_text = insert(self.input_text, self.cursor_index - 1, key)

    def blink(self) -> None:
        """Switch the blink color on the cursor."""
        self.is_white = not self.is_white

    def on_mount(self):
        self.set_interval(1, self.blink)  # type: ignore

    @staticmethod
    def make_text(input_text: str, is_white: bool, cursor_index: int) -> str:
        text: str = ""

        d = {False: "black on white", True: "white on black"}

        for index, value in enumerate(input_text + " "):
            if index == cursor_index:
                t = d[is_white] if not input_text else d[False]
                text += f"[{t}]{value}[/{t}]"
            else:
                text += escape(value)
        
        return text
