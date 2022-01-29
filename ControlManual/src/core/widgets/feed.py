from textual.reactive import Reactive
from textual.widget import Widget
from rich.panel import Panel
import logging

__all__ = ["Feed"]

class Feed(Widget):
    text = Reactive("")

    def print(self, *args) -> None:
        self.write(*args, '\n')

    def write(self, *args) -> None:
        logging.debug(f"write called with args: {args}")
        for i in args:
            self.text += str(i)

    def render(self):
        logging.debug("rendered feed")
        return Panel(self.text)