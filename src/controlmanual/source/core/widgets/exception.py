from textual.widget import Widget
from rich.panel import Panel
from textual.reactive import Reactive
from rich.pretty import Pretty
from rich.console import Group


__all__ = ["ExcPanel"]

class ExcPanel(Widget):
    """Widget for holding the debugger."""
    text = Reactive('No exceptions.')
    locals = Reactive({})

    def render(self) -> Panel:
        full = Group(self.text, Pretty(self.locals) if self.locals else '')

        return Panel(full, title = "Exception")