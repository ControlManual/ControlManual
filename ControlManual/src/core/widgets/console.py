from textual.widget import Widget
from textual.reactive import Reactive
from rich.panel import Panel
from textual.events import Key
from textual.keys import Keys
import logging
from typing import Callable, Coroutine
from rich.layout import Layout


Callback = Callable[[str], Coroutine[None, None, None]]

def insert(base: str, index: int, value: str) -> str:
    """Insert a string to an index."""
    l = list(base)
    l.insert(index, value)
    return ''.join(l)

def remove(base: str, index: int) -> str:
    """Remove a character at an index."""
    l = list(base)
    l.pop(index)
    return ''.join(l)

class Console(Widget):
    """Widget for representing the console interface."""
    input_text = Reactive('')
    is_white = Reactive(True)
    cursor_index = Reactive(0)
    callback: Callback
    title: str
    feed_text = Reactive('')

    def __init__(self, *args, **kwargs) -> None:
        self.callback = kwargs.pop('callback')
        
        super().__init__(*args, **kwargs)
        

    def render(self) -> Layout:
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

        l = Layout()
        l.split_column(Panel(self.feed_text, title = "Feed"), Panel(text, title = "Terminal"))

        return l

    async def on_key(self, event: Key) -> None:
        key: str = event.key
        logging.debug(f"keypress: {key}")

        if key in [key.value for key in Keys]:
            if key == Keys.Enter:
                await self.callback(self.input_text)
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
        """Switch the blink color on the cursor."""
        self.is_white = not self.is_white

    def on_mount(self):
        self.set_interval(1, self.blink)

    def print(self, *args) -> None:
        self.write(*args, '\n')

    def write(self, *args) -> None:
        logging.debug(f"write called with args: {args}")
        for i in args:
            self.feed_text += str(i)