from textual.widget import Widget
from rich.panel import Panel
from rich.layout import Layout
from .utils import Input

__all__ = ["Debugger"]

class Debugger(Widget, Input):
    """Widget for holding the debugger."""
    def render(self) -> Panel:
        l = Layout()
        l.split_column(Panel("abc"), Panel(Input.make_text(self.input_text, self.is_white, self.cursor_index)))
        return Panel(l, title = "Debugger")