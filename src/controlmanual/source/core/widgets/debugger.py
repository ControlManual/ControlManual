from rich.layout import Layout
from rich.panel import Panel
from textual.widget import Widget

from .utils import Input

__all__ = ["Debugger"]


class Debugger(Widget, Input):
    """Widget for holding the debugger."""

    def render(self) -> Panel:
        lay = Layout()
        lay.split_column(
            Panel("abc"),
            Panel(Input.make_text(self.input_text, self.is_white, self.cursor_index)),
        )
        return Panel(lay, title="Debugger")
