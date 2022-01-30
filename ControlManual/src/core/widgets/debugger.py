from textual.reactive import Reactive
from textual.widget import Widget
from rich.panel import Panel
from platform import python_version
from rich.layout import Layout

__all__ = ["Debugger"]

class Debugger(Widget):
    """Widget for holding the debugger."""
    def render(self) -> Panel:
        l = Layout()
        l.split_column(Panel("abc"), Panel("123"))
        return Panel(l, title = "Debugger")
        #return Panel(f"""Python Version: {python_version()}""", title = "Debugger")