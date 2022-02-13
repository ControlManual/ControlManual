import logging
from abc import ABC

from textual.events import Key
from textual.keys import Keys
from textual.reactive import Reactive


def insert(base: str, index: int, value: str) -> str:
    """Insert a string to an index."""
    lis = list(base)
    lis.insert(index, value)
    return "".join(lis)


def remove(base: str, index: int) -> str:
    """Remove a character at an index."""
    lis = list(base)
    lis.pop(index)
    return "".join(lis)


class Input(ABC):
    input_text = Reactive("")
    is_white = Reactive(True)
    cursor_index = Reactive(0)

    async def on_key(self, event: Key) -> None:
        key: str = event.key
        logging.debug(f"keypress: {key}")

        if key in [key.value for key in Keys]:
            if key == Keys.Enter:
                await self.callback(self.input_text)  # type: ignore
                self.input_text = " "
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
        self.set_interval(1, self.blink)  # type: ignore

    @staticmethod
    def make_text(input_text: str, is_white: bool, cursor_index: int) -> str:
        text: str = ""

        d = {False: "black on white", True: "white on black"}

        for index, value in enumerate(input_text + " "):
            if index == cursor_index:
                t = d[is_white]
                text += f"[{t}]{value}[/{t}]"
            else:
                text += value

        return text
