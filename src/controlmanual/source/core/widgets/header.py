from textual.widget import Widget
from textual.reactive import Reactive
from rich.text import Text


class Header(Widget):
    style: Reactive[str] = Reactive("black on white")
    title: Reactive[str] = Reactive("Control Manual")

    def render(self):
        return Text(self.title, style=self.style, justify="center")

    def on_mount(self):
        self.layout_size = 1
