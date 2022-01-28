from textual.app import App
from textual.reactive import Reactive
from textual.widget import Widget
from rich.panel import Panel
from textual.events import Key, Load
from textual.keys import Keys
from .client import Client
import asyncio
from .utils import run

"""
class Console(Widget):
    input_text = Reactive('')

    def render(self) -> Panel:
        return Panel(self.input_text, title = "Terminal")

    def on_key(self, event: Key) -> None:
        self.input_text += event.key
    

class Application(App):
    client = Client()

    async def on_mount(self) -> None:
        c = Console()

        await c.focus()
        await self.view.dock(c, edge = "right")

    async def on_load(self, event: Load):
        await self.bind(Keys.ControlC, "quit")


Application.run()

"""

class Application:
    client: Client

    async def __new__(cls):
        cls.client = await Client()
        return super().__new__(cls)

    async def start(self) -> None:
        while True:
            inp = input('>> ')

            await self.client.run_command(inp)

    def run(self):
        run(self.start())