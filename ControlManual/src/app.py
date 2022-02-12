from textual.app import App
from textual.events import Load
from textual.keys import Keys
from .client import Client
import logging
from .core.widgets import *
from textual.reactive import Reactive
from textual.widgets import TreeControl, TreeClick
import os
from .core.theme import console_object

__all__ = ["Application"]

DEBUG_LEN: int = 50
FS_LEN: int = 40

class Application(App):
    client: Client
    interface: Console

    show_bar = Reactive(False)
    filesystem = Reactive(False)

    def watch_show_bar(self, show_bar: bool) -> None:
        self.bar.animate("layout_offset_x", 0 if show_bar else -DEBUG_LEN)

    def action_toggle_sidebar(self) -> None:
        self.show_bar = not self.show_bar

    def watch_filesystem(self, filesystem: bool) -> None:
        self.filesystem_widget.animate("layout_offset_x", 0 if filesystem else -FS_LEN)

    def action_toggle_filesystem(self) -> None:
        self.filesystem = not self.filesystem

    async def on_mount(self) -> None:
        self.console = console_object
        self.client = await Client(self)
        self.bar = Debugger()
        self.filesystem_widget = TreeControl("Directory", "")
        logging.debug(f"{self.console}")

        for i in os.listdir(self.client.path):
            await self.filesystem_widget.add(self.filesystem_widget.root.id, i, i)

        await self.filesystem_widget.root.expand()

        c = Console(client = self.client)
        self.interface = c
        await c.focus()
        logging.info("focused console")
        
        await self.view.dock(self.filesystem_widget, edge = "left", size = FS_LEN, z = 1)
        await self.view.dock(self.bar, edge = "left", size = DEBUG_LEN, z = 2)
        await self.view.dock(RightBar(), edge = "right", size = 50)
        await self.view.dock(c, edge = "top")

        self.bar.layout_offset_x = -DEBUG_LEN
        self.filesystem_widget.layout_offset_x = -FS_LEN
        
        
        logging.info("docked widgets")

    async def on_load(self, event: Load):
        await self.bind(Keys.ControlC, "quit")
        await self.bind(Keys.ControlD, "toggle_sidebar")
        await self.bind(Keys.ControlF, "toggle_filesystem")

    async def handle_tree_click(self, message: TreeClick[str]) -> None:
        await self.client.run_command(f"path {message.node.data}")