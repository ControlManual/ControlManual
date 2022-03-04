from rich.layout import Layout
from rich.panel import Panel
from rich.console import Console, ConsoleOptions, RenderResult
from textual.widget import Widget
from ...logger import log_path

from .utils import Input

__all__ = ["Debugger"]


class LogPanel:
    def __rich_console__(
        self, console: Console, options: ConsoleOptions
    ) -> RenderResult:
        with open(log_path) as f:
            yield Panel(
                '\n'
                .join(
                    f.read()
                    .split('\n')
                    [-options.max_height:]
                ),
                title = "Logs",
            )

class Debugger(Widget, Input):
    """Widget for holding the debugger."""

    def render(self) -> Panel:
        lay = Layout()
        lay.split_column(
            LogPanel(),
            Panel(Input.make_text(self.input_text, self.is_white, self.cursor_index)),
        )
            
        return Panel(lay, title="Debugger")
