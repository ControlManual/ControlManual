import asyncio
import logging
import os
import traceback

from textual.app import App
from textual.keys import Keys
from textual.reactive import Reactive
from textual.widgets import TreeClick, TreeControl, ScrollView

from .client import Client
from .core.theme import console_object
from .core.widgets import Console, Debugger, ExcPanel, RightBar, Header
from .utils import not_null
from rich.syntax import Syntax
from .core.config import config

__all__ = ["Application"]

DEBUG_LEN: int = 50
FS_LEN: int = 40
EXC_LEN: int = 100


def MAKE_SYN(x): return Syntax.from_path(x, line_numbers=True, theme=config["read_theme"])


class Application(App):
    client: Client
    interface: Console

    show_bar = Reactive(False)
    filesystem = Reactive(False)
    excpanel = Reactive(False)

    def watch_show_bar(self, show_bar: bool) -> None:
        self.bar.animate("layout_offset_x", 0 if show_bar else -DEBUG_LEN)

    def action_toggle_sidebar(self) -> None:
        self.show_bar = not self.show_bar

    def watch_filesystem(self, filesystem: bool) -> None:
        self.filesystem_widget.animate("layout_offset_x", 0 if filesystem else -FS_LEN)

    def action_toggle_filesystem(self) -> None:
        self.filesystem = not self.filesystem

    def watch_excpanel(self, excpanel: bool) -> None:
        self.exc_panel.animate("layout_offset_x", 0 if excpanel else EXC_LEN)

    def action_toggle_excpanel(self) -> None:
        self.excpanel = not self.excpanel

    async def set_syntax(self, path: str) -> None:
        await self.syntax_panel.update(MAKE_SYN(path))

    def action_toggle_syntax(self):
        current = self.syntax_panel.visible

        self.header.title = "Press CTRL+S to exit." if not current else "Control Manual"
        self.header.visible = not current
        self.syntax_panel.visible = not current

    async def on_mount(self) -> None:
        self.console = console_object
        self.client = Client()
        await self.client.init(self)  # because mypy was angry

        self.filesystem_widget = TreeControl(str(self.client.path), "")
        for i in os.listdir(self.client.path):
            if os.path.isdir(os.path.join(self.client.path, i)):
                await self.filesystem_widget.add(self.filesystem_widget.root.id, i, i)

        await self.filesystem_widget.root.expand()

        self.bar = Debugger()
        self.exc_panel = ExcPanel()
        logging.debug(f"{self.console}")
        self.syntax_panel = ScrollView(MAKE_SYN(os.devnull))
        self.syntax_panel.visible = False

        c = Console(client=self.client)
        self.interface = c
        await c.focus()
        logging.info("focused console")
        self.header = Header(name="header")
        self.header.visible = False

        await self.view.dock(self.header)
        await self.view.dock(self.filesystem_widget, edge="left", size=FS_LEN, z=1)
        await self.view.dock(self.exc_panel, edge="right", size=EXC_LEN, z=1)
        await self.view.dock(self.bar, edge="left", size=DEBUG_LEN, z=2)
        await self.view.dock(self.syntax_panel, size=100, edge="top", name="syntax")
        await self.view.dock(RightBar("rightbar"), edge="right", size=50)
        await self.view.dock(c, edge="top")

        self.bar.layout_offset_x = -DEBUG_LEN
        self.filesystem_widget.layout_offset_x = -FS_LEN
        self.exc_panel.layout_offset_x = EXC_LEN

        logging.info("docked widgets")

    async def on_load(self):
        await self.bind(Keys.ControlC, "quit")
        await self.bind(Keys.ControlD, "toggle_sidebar")
        await self.bind(Keys.ControlF, "toggle_filesystem")
        await self.bind(Keys.ControlE, "toggle_excpanel")
        await self.bind(Keys.ControlS, "toggle_syntax")
        await self.bind(Keys.ControlR, "view.toggle('rightbar')")

    async def handle_tree_click(self, message: TreeClick[str]) -> None:
        await self.client.run_command(f"path {message.node.data}")

    async def show_exc(self, exc: Exception):
        self.exc_panel.text = (
            "[error]"
            + "".join(
                traceback.format_exception(
                    etype=type(exc), value=exc, tb=exc.__traceback__
                )
            )
            + "\n[/error]"
        )
        frame = not_null(exc.__traceback__).tb_frame

        self.exc_panel.locals = frame.f_locals

        self.action_toggle_excpanel()
        asyncio.create_task(self.auto_close_excpanel())

    async def auto_close_excpanel(self):
        await asyncio.sleep(5)
        self.action_toggle_excpanel()
