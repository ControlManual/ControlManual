from textual.app import App
from textual.events import Load
from textual.keys import Keys
from .client import Client
import logging
from .core.widgets import *
from textual.reactive import Reactive

__all__ = ["Application"]

DEBUG_LEN: int = 50

class Application(App):
    client: Client
    interface: Console

    show_bar = Reactive(False)

    def watch_show_bar(self, show_bar: bool) -> None:
        """Called when show_bar changes."""
        self.bar.animate("layout_offset_x", 0 if show_bar else -DEBUG_LEN)

    def action_toggle_sidebar(self) -> None:
        """Called when user hits 'b' key."""
        self.show_bar = not self.show_bar


    async def on_mount(self) -> None:
        self.client = await Client(self)
        self.bar = Debugger()

        c = Console(callback = self.client.run_command)
        self.interface = c
        await c.focus()
        logging.info("focused console")
        
        await self.view.dock(self.bar, edge = "left", size = DEBUG_LEN, z = 1)
        await self.view.dock(RightBar(), edge = "right", size = 35)
        await self.view.dock(c, edge = "top")

        self.bar.layout_offset_x = -DEBUG_LEN
        
        
        logging.info("docked widgets")

    async def on_load(self, event: Load):
        await self.bind(Keys.ControlC, "quit")
        await self.bind(Keys.ControlD, "toggle_sidebar")
