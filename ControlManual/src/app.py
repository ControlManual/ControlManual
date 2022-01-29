from textual.app import App
from textual.reactive import Reactive
from textual.widget import Widget
from rich.panel import Panel
from textual.events import Key, Load
from textual.keys import Keys
from .client import Client
import logging

__all__ = ["Application"]

def insert(base: str, index: int, value: str) -> str:
    l = list(base)
    l.insert(index, value)
    return ''.join(l)

def remove(base: str, index: int) -> str:
    l = list(base)
    l.pop(index)
    return ''.join(l)

class Console(Widget):
    input_text = Reactive('')
    is_white = Reactive(True)
    cursor_index = Reactive(0)
    client: Client

    async def register_client(self, client: Client) -> None:
        self.client = client

    def render(self) -> Panel:
        d = {
            False: 'black on white',
            True: 'white on black'
        }
        text: str = ''

        for index, value in enumerate(self.input_text + ' '):
            if index == self.cursor_index:
                t = d[self.is_white]
                text += f'[{t}]{value}[/{t}]'
            else:
                text += value

        return Panel(text, title = "Terminal")

    async def on_key(self, event: Key) -> None:
        key: str = event.key

        if key in [Keys.Enter, Keys.Left, Keys.Right, Keys.Escape, Keys.ControlC, Keys.ControlH]:
            if key == Keys.Enter:
                await self.client.run_command(self.input_text)
                self.input_text = ' '
                self.cursor_index = 0

            if key == Keys.Left and self.cursor_index:
                self.cursor_index -= 1

            if key == Keys.Right and (self.cursor_index) != len(self.input_text):
                self.cursor_index += 1

            if key == Keys.ControlH and self.cursor_index:
                self.cursor_index -= 1
                self.input_text = remove(self.input_text, self.cursor_index)

            return

        self.cursor_index += 1
        self.input_text = insert(self.input_text, self.cursor_index - 1, key)
    
    def blink(self) -> None:
        self.is_white = not self.is_white

    def on_mount(self):
        self.set_interval(1, self.blink)

class Application(App):
    client: Client

    async def init(self):
        self.client = await Client(self)

    async def on_mount(self) -> None:
        c = Console()

        await c.focus()
        await self.view.dock(c, edge = "right")

    async def on_load(self, event: Load):
        await self.bind(Keys.ControlC, "quit")