from textual.widget import Widget
from textual.reactive import Reactive
from rich.panel import Panel
from textual.events import Key
from textual.keys import Keys
import logging
from typing import Callable, Coroutine
from rich.layout import Layout
import psutil
from rich.table import Table
from rich import box
import socket
from socket import AF_INET, SOCK_DGRAM, SOCK_STREAM
from rich.align import Align
from ...client import Client
from .utils import Input

AD = "-"
AF_INET6 = getattr(socket, 'AF_INET6', object())
proto_map = {
    (AF_INET, SOCK_STREAM): 'tcp',
    (AF_INET6, SOCK_STREAM): 'tcp6',
    (AF_INET, SOCK_DGRAM): 'udp',
    (AF_INET6, SOCK_DGRAM): 'udp6',
}

Callback = Callable[[str], Coroutine[None, None, None]]

__all__ = ["Console"]

class Console(Widget, Input):
    """Widget for representing the console interface."""
    callback: Callback
    title: str
    feed_text = Reactive('')
    client: Client

    def __init__(self, *args, **kwargs) -> None:
        self.client = kwargs.pop('client')
        self.callback = self.client.run_command
        
        super().__init__(*args, **kwargs)
        

    def render(self) -> Layout:
        table = Table(box = box.SIMPLE)

        table.add_column("Protocol")
        table.add_column("Local Address")
        table.add_column("Remote Address")
        table.add_column("Status")
        table.add_column("Process")
        table.add_column("PID")

        proc_names = {
            p.pid: p.name
            for p in psutil.process_iter(['pid', 'name'])
        }

        imp = lambda x: f"[important]{x}[/important]"

        for conn in psutil.net_connections():
            name = proc_names.get(conn.pid, '?') or ''
            laddr = imp("%s:%s") % (conn.laddr)
            raddr = imp("%s:%s") % (conn.raddr) if conn.raddr else ""
            table.add_row(imp(proto_map[(conn.family, conn.type)]), laddr, raddr or imp(AD), imp(conn.status), imp(name if isinstance(name, str) else name()), imp(conn.pid))


        text: str = Input.make_text(self.input_text, self.is_white, self.cursor_index)

        l = Layout()
        l.split_column(
            Panel
            (
                f"{self.client.path} [success]{self.client.config['input_sep']}[/success] " + text + "\n" + self.feed_text, 
                title = "Terminal"
            ), 
            Panel(Align.center(table), title = "Connections")
        )

        return l

    def print(self, *args) -> None:
        self.write(*args, '\n')

    def write(self, *args) -> None:
        logging.debug(f"write called with args: {args}")
        for i in args:
            self.feed_text += str(i)