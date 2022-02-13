from rich.console import Group
from rich.panel import Panel
from rich.pretty import Pretty
from textual.reactive import Reactive
from textual.widget import Widget

__all__ = ["ExcPanel"]


class ExcPanel(Widget):
    """Widget for holding the debugger."""

    text = Reactive("No exceptions.")
    locals = Reactive({})

    def render(self) -> Panel:
        full = Group(self.text, Pretty(self.locals) if self.locals else "")

        return Panel(full, title="Exception")
