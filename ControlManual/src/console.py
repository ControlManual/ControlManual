import os
from textual import events
from textual.app import App
from textual.widgets import Footer, ScrollView, DirectoryTree
from rich.text import Text
from rich.padding import Padding
from typing import Coroutine, Any
from .config import Config
from rich.theme import Theme
from rich.console import Console
import asyncio

config = Config()


truecolor: bool = config.truecolor

PRIMARY: str = "rgb(0,179,0) on black"


custom_theme = Theme({
    "danger": "bold red",
    "primary": PRIMARY if truecolor else "bold green",
    "secondary": "rgb(21,128,0) on black" if truecolor else "dim green",
    "important": f"bold {PRIMARY}",
    "grayed": "rgb(61,61,61)",
    "disabled": "rgb(77,77,77)"
})


class ConsoleApp(App):
    async def on_load(self, event: events.Load) -> None:
        """Bind keys with the app loads (but before entering application mode)"""
        await self.bind("b", "view.toggle('sidebar')", "Toggle sidebar")
        await self.bind("q", "quit", "Quit")
        self.text = ""
        self.allow_input = False

    async def on_mount(self, event: events.Mount) -> None:
        """Create and dock the widgets."""

        dirs = ScrollView(DirectoryTree("/home/zero"))
        body = ScrollView(await self.add_text(""))
        p = ScrollView("A: B\nA: B\nA: B\nA: B\nA: B\nA: B\nA: B")
        self.body = body

        await self.view.dock(Footer(), edge = "bottom")
        await self.view.dock(dirs, edge = "left", size = 30, name = "sidebar")
        await self.view.dock(p, edge = "right", size = 30)
        await self.view.dock(body, edge = "top", name = "terminal")

    def attach_client(self, client) -> None:
        self.client_class = client

    async def add_text(self, text: str):
        self.text += text
        return self.text

    async def next_input(self):
        c = self.client_class
        await self.add_text(f"[white]{c.path}[/white] [green]{c.config.input_set}[/green]")

    async def on_key(self, event: events.Key):
        if not self.allow_input:
            return
        
        await self.next_input()
        
        key: str = event.key

        if key == "enter":
            await self.client_class.passenger(self.text)

        await self.body.update(await self.add_text(key))
class ConsoleWrapper:
    async def __new__(cls):
        self = super().__new__(cls)
        await cls.init(self)
        return self 

    async def init(self) -> None:
        self._app = ConsoleApp(console = Console(theme = custom_theme), screen = True, driver_class = None)
        await self._app.process_messages()
    
    @property
    def app(self):
        """Textual app class."""
        return self._app

    async def print(self, *text: str) -> None:
        await self.write(*text, "\n")

    async def write(self, *text: str) -> None:
        t = " ".join(text)
        await self._app.add_text(t)

    async def allow_input(self) -> None:
        self._app.allow_input = True

    async def lock_input(self) -> None:
        self._app.allow_input = True

    def attach_client(self, client) -> None:
        self._app.client_class = client 

    async def error(self, e: str) -> None:
        await self.print(f'[danger]Error:[/danger] {e}')
    
    async def success(self, s: str) -> None:
        await self.print(f'[primary]Success:[/primary] {s}')


def console_factory():
    loop = asyncio.get_event_loop()
    console = loop.run_until_complete(ConsoleWrapper())

    return console

console = console_factory()