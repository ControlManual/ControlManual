import socket
from socket import AF_INET, SOCK_DGRAM, SOCK_STREAM
from typing import Any, Callable, Coroutine

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

from ...client import Client
from .utils import Input

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


def lower(x): return x.lower() if config["lowercase"] else x


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


class Console(Widget, Input):
    """Widget for representing the console interface."""

    callback: Callback
    title: str
    feed_text = Reactive("")
    client: Client

    def __init__(self, *args: Any, **kwargs: Any) -> None:
        self.client = kwargs.pop("client")
        self._console_client = ConsoleClient(self)
        self.callback = self.client.run_command  # type: ignore

        super().__init__(*args, **kwargs)

    def render(self) -> Layout:
        table = Table(box=box.SIMPLE)

        for x in ("Protocol", "Local Address", "Remote Address", "Status", "PID"):
            table.add_column(lower(x))

        proc_names = {p.pid: p.name for p in psutil.process_iter(["pid", "name"])}

        def imp(x):
            return f"[important]{x}[/important]"

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

        text: str = Input.make_text(self.input_text, self.is_white, self.cursor_index)

        lay = Layout()

        lay.split_column(
            Panel(
                f"{self.client.path} [success]{self.client.config['input_sep']}[/success] "
                + text
                + "\n"
                + self.feed_text,
                title="Terminal",
            ),
            Panel(Align.center(table), title="Connections"),
        )

        return lay

    @property
    def console_client(self):
        return self._console_client
