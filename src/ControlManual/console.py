from rich.console import Console
from rich.theme import Theme
from rich.layout import Layout
from rich.panel import Panel
import os
from .config import Config
from typing import Literal, Tuple, Any, Optional, Dict, overload, Union
from getch import getch


primary: str = "rgb(0,179,0) on black"
config = Config()
basic: bool = config.basic

truecolor: bool = config.truecolor

custom_theme = Theme({
    "danger": "bold red",
    "primary": primary if truecolor else "bold green",
    "secondary": "rgb(21,128,0) on black" if truecolor else "dim green",
    "important": f"bold {primary}",
    "grayed": "rgb(61,61,61)",
    "disabled": "rgb(77,77,77)"
})


@overload
def make_panel(name: str, force: bool = True) -> Layout:
    ...


@overload
def make_panel(
        name: str,
        force: bool = False
) -> Optional[Layout]:  # overloads are weird sometimes
    ...


def make_panel(name: str, force: bool = False) -> Optional[Layout]:
    c: str = name.capitalize()
    text: str = f"{c} will show here..."

    if (name in config.columns) or (force):
        return Layout(Panel(text, title=c), name=name)


class ConsoleWrapper:
    """Class wrapping around the Rich Console API."""
    def __init__(self):
        self._console = Console(
            color_system="truecolor"
            if config.truecolor else "standard" if config.colorize else None,
            theme=custom_theme,
        )
        layout = Layout()

        layout.split_row(make_panel("feed", True), Layout(name="lower"))
        self.render(
            layout["lower"],
            "column",
            make_panel("info"),
            Layout(name="command_data"),
            Layout(name="small"),
        )

        self.render(layout["command_data"], "row", make_panel("exceptions"))

        self.render(layout["small"], "row", make_panel("directory"),
                    make_panel("log"))

        self._screen = layout
        self._feed = []
        self._amount = 0

    def render(self, layout: Layout, typ: Literal["row", "column"],
               *rows: Optional[Layout]):
        """Internal utility method. Used for rendering panels to the screen."""

        if typ not in ["row", "column"]:
            raise ValueError(
                "typ must be row or column")  # mainly for safety purposes

        target = layout.split_row if typ == "row" else layout.split_column

        r = list(rows)

        for i in r:
            if i is None:
                r.remove(i)

        target(*r)  # type: ignore

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
        self.clear_panel("feed")

    def print(self, message: Any):
        """Function for printing a message."""
        prefix: str = "\n" if self._feed else ""
        self.write(prefix + str(message))

    def write(self, message: Any):
        """Function for writing to the feed."""
        f = self._feed
        suffix: str = ""
        amount_string = lambda a: f" [important]x{a}[/important]"
        message = str(message)

        if message == f[-1] if f else None:
            self._amount += 1
            suffix: str = amount_string(self._amount)
        else:
            if self._amount:
                f[-1] = f[-1] + amount_string(self._amount)
                self._amount = 0
            self._feed.append(message)

        self.edit_panel("feed", "".join(f) + suffix)

    def error(self, message: Any, *args, **kwargs) -> None:
        self.print(f"[danger]Error:[/danger] {message}", *args, **kwargs)

    def success(self, message: Any, *args: Tuple[str],
                **kwargs: Dict[str, str]) -> None:
        self.print(f"[important]Success:[/important] {message}", *args,
                   **kwargs)

    def set_info(self, text: str) -> None:
        self.edit_panel("info", text)

    def edit_panel(self, panel: str, text: str) -> None:
        self.screen[panel].update(Panel(text, title=panel.capitalize()))

    def show_exc(self, error: Exception):
        self.edit_panel("exceptions", repr(error))

    def key_value(self, key: str, value: str) -> None:
        self.print(
            f"[primary]{key}[/primary] - [secondary]{value}[/secondary]")

    def set_dir(self, path: Union[os.PathLike, str]) -> None:
        final: str = ""

        for i in os.listdir(path):
            if os.path.isfile(os.path.join(path, i)):
                c: str = "primary"
            else:
                c: str = "secondary"

            final += f"[{c}]{i}[/{c}] "

        self.edit_panel("directory", final)

    def clear_panel(self, panel: str) -> None:
        self.edit_panel(panel, f"{panel.capitalize()} will show here...")

    def primary(self, message: str, *args, **kwargs) -> None:
        self.print(f"[primary]{message}[/primary]", *args, **kwargs)

    def secondary(self, message: str, *args, **kwargs) -> None:
        self.print(f"[secondary]{message}[/secondary]", *args, **kwargs)

    @staticmethod
    def clear() -> None:
        """Cross platform function for clearing terminal window."""
        if os.name == "nt":  # if the platform is windows
            print(chr(27) + "[2J", end="")  # clears screen for windows
        else:  # for anything other than windows
            print("\033c", end="")  # clears screen for linux and mac

    @staticmethod
    def clear_autocomplete(current: str) -> Literal['']:
        print(' ' * len(current), end = "")
        print('\b' * len(current), flush = True, end = "")
        return ''

    def set_highlight(self, text: str, color: str) -> Literal[True]:
        print("\b" * len(text), flush = True, end = "")
        self.console.print(f"[{color}]{text}[/{color}]", end = "")
        return True

    def take_input(self, prompt: str, commands: dict, aliases: dict) -> str:
        """Render a new screen frame and take input."""
        c = self.console
        self.clear()
        p = Panel(self.screen, title = "Terminal", height = self.console.height - 1)
        c.print(p)

        c.print(prompt, end = "")
        string: str = ''
        highlighted: bool = False
        current_autocomplete: str = ''
        both = {**commands, **aliases}
        main_color: str = 'white'
        comment = False
        is_flag: bool = False

        while True:
            for i in both:
                if string:
                    split = string.split(' ')
                    cmd = split[0]

                    if cmd in config.comments:
                        if not comment:
                            main_color: str = 'disabled'
                            highlighted = self.set_highlight(string, 'disabled')
                            current_autocomplete = self.clear_autocomplete(current_autocomplete)
                            comment = True
                        break
                    else:
                        main_color: str = 'white'
                        comment = False

                    if string in config.functions:
                        main_color: str = 'important'
                        highlighted = self.set_highlight(string, 'important')
                        current_autocomplete = self.clear_autocomplete(current_autocomplete)
                        break
                    else:
                        main_color: str = 'white'

                    if len(split) > 1:
                        if cmd not in both:
                            current_autocomplete = self.clear_autocomplete(current_autocomplete)
                            highlighted = self.set_highlight(string, 'danger')
                            break

                        if split[-1].startswith(config.flag_prefix):
                            main_color: str = 'secondary'
                            is_flag = True

                    if cmd in both:
                        if not highlighted:
                            if cmd in commands:
                                if 'exe' in commands[cmd]:
                                    color: str = "secondary"
                                else:
                                    color: str = "primary"
                            else:
                                color: str = "secondary"        

                            highlighted = self.set_highlight(string, color)
                            current_autocomplete = self.clear_autocomplete(current_autocomplete)
                            break
                    elif highlighted:
                        print("\b" * len(string), flush = True, end = "")
                        c.print(f"[white]{string}[/white]", end = "")
                        highlighted = False

                    if i.startswith(string):
                        if not highlighted:
                            self.clear_autocomplete(current_autocomplete)

                            append: str = i[len(string):]

                            c.print(f"[grayed]{append}[/grayed]", end = "")
                            print("\b" * len(append), flush = True, end = "")
                            current_autocomplete = i
                            break
                        else:
                            current_autocomplete = self.clear_autocomplete(current_autocomplete)
                    elif current_autocomplete:
                        current_autocomplete = self.clear_autocomplete(current_autocomplete)
                else:
                    current_autocomplete = self.clear_autocomplete(current_autocomplete)           
 
                        

            char: str = getch()

            if char == '\n':
                break
            
            if char == '\x7f':
                if string:
                    string = string[:-1]
                    print('\b \b', end = '', flush = True)
            elif ord(char) == 27:
                pass
            else:
                col = main_color
                if is_flag and (char == '-'):
                    col = 'white'
                
                if char == ' ':
                    is_flag = False

                c.print(f'[{col}]{char}', end = '')
                string += char

        return string

console = ConsoleWrapper()
