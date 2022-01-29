from textual.app import App
from textual.events import Load
from textual.keys import Keys
from .client import Client
import logging
from .core.widgets import *

__all__ = ["Application"]

class Application(App):
    client: Client
    feed: Feed

    async def on_mount(self) -> None:
        self.client = await Client(self)
        self.feed = Feed()

        c = Console()
        c.register_client(self.client)
        await c.focus()
        logging.info("focused console")
        await self.view.dock(self.feed, c, edge = "right")
        logging.info("docked widgets")

    async def on_load(self, event: Load):
        await self.bind(Keys.ControlC, "quit")