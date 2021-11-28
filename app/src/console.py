from rich.console import Console
from rich.theme import Theme
from rich.layout import Layout
from rich.panel import Panel
import os
from .config import Config
from typing import Literal, Type

custom_theme = Theme({
    "danger": "bold red",
    "primary": "rgb(0,179,0) on black" if not os.name == 'nt' else 'green',
    "secondary": "rgb(21,128,0) on black" if not os.name == 'nt' else 'dim green'
})

config = Config()

def make_panel(name: str) -> Layout:
    c: str = name.capitalize()
    text: str = f'{c} will show here...'
    if (name in config.columns) or (name == "feed"):
        return Layout(Panel(text, title = c), name = name)

def render(layout: Layout, typ: Literal["row", "column"], *rows):
    if typ not in ["row", "column"]:
        raise ValueError("typ must be row or column") # mainly for safety purposes
    
    target = layout.split_row if typ == "row" else layout.split_column

    r = list(rows)
    for i in r:
        if i is None:
            r.remove(i)
    
    target(*r)


class ConsoleWrapper:
    """Class wrapping around the Rich Console API."""
    def __init__(self):
        colorsys: str = 'truecolor' if not os.name == 'nt' else 'standard'
        self._console = Console(color_system = colorsys if config.colorize else None, theme = custom_theme)
        layout = Layout()
        layout.split_row(
            make_panel("feed"),
            Layout(name="lower")
        )
        render(layout["lower"], "column",
            make_panel("info"),
            Layout(name = "command_data"),
            Layout(name = "small")
        )

        render(layout["command_data"], "row", 
            make_panel("data"),
            make_panel("exceptions")
        )

        render(layout["small"], "row",
            make_panel("directory"),
            make_panel("log")
        )

        self._screen = layout
        self._feed = []
        self._amount = 0

    @property
    def console(self) -> Console:
        """Raw Console Object."""
        return self._console
    
    @property
    def screen(self) -> Layout:
        """Raw Rich Layout Object."""
        return self._screen

    def empty(self) -> None:
        """Empty the feed."""
        self._feed = []
        self.edit_panel("feed", "")

    def print(self, message: str):
        """Function for writing to the feed."""
        f = self._feed
        end: str = ''
        amount_string: str = lambda a: f' [primary]x{a}[/primary]'
        
        if message == f[-1] if f else None:
            self._amount += 1
            end: str = amount_string(self._amount)
        else:
            if self._amount:
                f[-1] = f[-1] + amount_string(self._amount)
                self._amount = 0
            self._feed.append(message)
        
        self.edit_panel("feed", f'\n'.join(f) + end)
    
    def error(self, message: str) -> None:
        self.print(f"[danger]Error:[/danger] {message}")

    def success(self, message: str) -> None:
        self.print(f"[primary]Success:[/primary] {message}")
    
    def set_info(self, text: str) -> None:
        self.edit_panel("info", text)

    def set_data(self, data: dict) -> None:
        final: str = ''
        
        for key, value in data.items():
            if isinstance(key, bool):
                c = 'primary' if key else 'danger'
                key = f'[{c}]{key}[/{c}]'
            final += f'{key}: {value}\n'

        self.edit_panel("data", final)

    def edit_panel(self, panel: str, text: str) -> None:
        self.screen[panel].update(Panel(text, title = panel.capitalize()))

    def show_exc(self, error: Type[Exception]):
        self.edit_panel("exceptions", repr(error))

    def key_value(self, key: str, value: str) -> None:
        self.print(f'[primary]{key}[/primary] - [secondary]{value}[/secondary]')

    def set_dir(self, path: str) -> None:
        final: str = ''

        for i in os.listdir(path):
            if os.path.isfile(os.path.join(path, i)):
                c: str = 'primary'
            else:
                c: str = 'secondary'
            
            final += f'[{c}]{i}[/{c}] '

        self.edit_panel("directory", final)

    def clear_panel(self, panel: str) -> None:
        self.edit_panel(panel, f'{panel.capitalize()} will show here...')

    def primary(self, message: str) -> None:
        self.print(f'[primary]{message}[/primary]')
    
    def secondary(self, message: str) -> None:
        self.print(f'[secondary]{message}[/secondary]')

    @staticmethod
    def clear() -> None:
        """Cross platform function for clearing terminal window."""
        if os.name == 'nt': # if the platform is windows
            print(chr(27) + "[2J", end="") # clears screen for windows
        else: # for anything other than windows
            print("\033c", end="") # clears screen for linux and mac

    def take_input(self, prompt: str) -> str:
        """Begin rendering the screen."""
        c = self.console
        self.clear()
        p = Panel(self.screen, title = "Terminal", height = self.console.height - 1)
        c.print(p)
        return c.input(prompt)

console = ConsoleWrapper()