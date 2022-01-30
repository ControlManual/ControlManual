from textual.reactive import Reactive
from textual.widget import Widget
from rich.panel import Panel
from platform import python_version

__all__ = ["Debugger"]

class Debugger(Widget):
    def render(self) -> Panel:
        return Panel(f"""Python Version: {python_version()}""", title = "Debugger")