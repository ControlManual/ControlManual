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
    """Function for making a panel."""
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
        self._command_history = []
        self._feed_height = self.console.height - 5

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

    @property
    def feed_height(self) -> int:
        """Height of the feed panel."""
        return self._feed_height

    def empty(self) -> None:
        """Empty the feed."""
        self._feed = []
        self.clear_panel("feed")

    def print(self, message: Any):
        """Function for printing a message."""
        self.write(str(message) + "\n")

    def write(self, message: Any):
        """Function for writing to the feed."""
        self._feed_height = self.console.height - 5
        f = self._feed


        suffix: str = ""
        amount_string = lambda a: f" [important]x{a}[/important]\n"
        message = str(message)
        tmp = message[:-1] if message.endswith("\n") else message

        last = f[-1] if f else None

        if (tmp == last) or (message == last):
            if f[-1].endswith('\n'):
                f[-1] = f[-1][:-1]
            self._amount += 1
            suffix: str = amount_string(self._amount)
        else:
            if self._amount:
                f[-1] = f[-1] + amount_string(self._amount)
                self._amount = 0
            self._feed.append(message)

            count = sum(i.count('\n') for i in f)
            if count >= self.feed_height:
                if f: # edge case
                    for i in range(count - self.feed_height + 2):
                        f.pop(0)

        lines = "".join(f) + suffix
        self.edit_panel("feed", lines)

    def error(self, message: Any, *args, **kwargs) -> None:
        """Function for printing an error message to the feed."""
        self.print(f"[danger]Error:[/danger] {message}", *args, **kwargs)

    def success(self, message: Any, *args: Tuple[str],
                **kwargs: Dict[str, str]) -> None:
        """Function for printing a success message to the feed."""
        self.print(f"[important]Success:[/important] {message}", *args,
                   **kwargs)

    def set_info(self, text: str) -> None:
        """Function for setting text on the info panel."""
        self.edit_panel("info", text)

    def edit_panel(self, panel: str, text: str) -> None:
        """Function for editing a panels text."""
        self.screen[panel].update(Panel(text, title = panel.capitalize()))

    def show_exc(self, error: Exception):
        """Function for adding an exception to the exceptions panel."""
        self.edit_panel("exceptions", repr(error))

    def key_value(self, key: str, value: str) -> None:
        """Function for printing a key value pair to the feed."""
        self.print(
            f"[primary]{key}[/primary] - [secondary]{value}[/secondary]")

    def set_dir(self, path: Union[os.PathLike, str]) -> None:
        """Function for setting a directory to the directory panel."""
        final: str = ""

        for i in os.listdir(path):
            if os.path.isfile(os.path.join(path, i)):
                c: str = "primary"
            else:
                c: str = "secondary"

            final += f"[{c}]{i}[/{c}] "

        self.edit_panel("directory", final)

    def clear_panel(self, panel: str) -> None:
        """Function for clearing a certain panel."""
        self.edit_panel(panel, f"{panel.capitalize()} will show here...")

    def primary(self, message: str, *args, **kwargs) -> None:
        """Function for printing a message in primary color."""
        self.print(f"[primary]{message}[/primary]", *args, **kwargs)

    def secondary(self, message: str, *args, **kwargs) -> None:
        """Function for printing a message in secondary color."""
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
        """Function for clearing the autocomplete text."""
        print(' ' * len(current), end = "")
        print('\b' * len(current), flush = True, end = "")
        return ''

    def set_highlight(self, text: str, color: str) -> Literal[True]:
        """Function for setting the syntax highlight for the input."""
        print("\b" * len(text), flush = True, end = "")
        self.console.print(f"[{color}]{text}[/{color}]", end = "")
        return True

    def set_autocomplete(self, current: str, full: str) -> str:
        """Function for setting the autocompletion text for the input."""
        append: str = full[len(current):]
        self.console.print(f"[grayed]{append}[/grayed]", end = "")
        print("\b" * len(append), flush = True, end = "")
        return full

    def get_terminal(self) -> Panel:
        """Function for getting the terminal panel."""
        return Panel(self.screen, title = "Terminal", height = self.console.height - 1)

    def render_screen(self) -> None:
        """Function for rendering a new screen frame."""
        c = self.console
        self.clear()
        p = self.get_terminal()
        c.print(p)

    def clear_highlight(self, text: str) -> tuple:
        """Function for clearing the highlighted text."""
        self.fw("\b" * len(text))
        self.console.print(f"[white]{text}[/white]", end = "")

        return False, ''

    @staticmethod
    def get_key():
        """Function for getting a character from the user."""
        first_char = getch()
        if first_char == '\x1b':
            return {'[A': 'up', '[B': 'down', '[C': 'right', '[D': 'left'}[getch() + getch()]
        else:
            return first_char

    def move(self, string: str):
        """Function for moving the text."""
        fw = self.fw

        fw(' ' * len(string), '\b' * len(string))
        fw('\b' + string)
        fw('\b' * (len(string) + 0)) # ? this only works if you do (len(ap) + 0) and not len(ap) wtf python

    @staticmethod
    def fw(*args) -> None:
        print(*args, end = '', flush = True)

    def take_input(self, prompt: str, commands: dict, aliases: dict) -> str:
        """Render a new screen frame and take input."""
        c = self.console
        self.render_screen()
        fw = self.fw

        current_command_history: int = 0
        c.print(prompt, end = "")
        string: str = ''
        highlighted: bool = False
        current_autocomplete: str = ''
        both = {'help': {}, **commands, **aliases}
        main_color: str = 'white'
        comment = False
        is_flag: bool = False
        highlight_type: str = ''
        index = 0

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

                    if (string in config.functions) or (string in [str(i) for i in range(1, 11)]):
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
                        if string[:-1] in both:
                            highlighted = False
                        if not highlighted:
                            if cmd in commands:
                                if 'exe' in commands[cmd]:
                                    highlight_type: str = "secondary"
                                else:
                                    highlight_type: str = "primary"
                            else:
                                highlight_type: str = "secondary"        

                            highlighted = self.set_highlight(string, highlight_type)
                            current_autocomplete = self.clear_autocomplete(current_autocomplete)
                            break
                    elif highlighted:
                        highlighted, highlight_type = self.clear_highlight(string)

                    if i.startswith(string):
                        if not highlighted:
                            self.clear_autocomplete(current_autocomplete)

                            current_autocomplete = self.set_autocomplete(string, i)
                            break
                        else:
                            current_autocomplete = self.clear_autocomplete(current_autocomplete)
                    elif current_autocomplete:
                        current_autocomplete = self.clear_autocomplete(current_autocomplete)
                else:
                    current_autocomplete = self.clear_autocomplete(current_autocomplete)           

            char: str = self.get_key()

            if char == '\n':
                if not string == (self._command_history + [None])[0]:
                    self._command_history.insert(0, string)
                break

            if char == '\x7f':
                if string:
                    string = string[:index - 1] + string[index:]
                    fw('\b \b')

                    if index < (len(string) - 1):
                        self.move(string[index - 1:])
                    index -= 1
            elif char == 'up':
                if not (current_command_history == len(self._command_history)):
                    fw('\b' * len(string))
                    string = self._command_history[current_command_history]
                    current_command_history += 1
                    current_autocomplete = self.clear_autocomplete(current_autocomplete)
                    fw(string)
                    index = len(string) - 1
                    
            elif char == 'down':
                if current_command_history:
                    current_command_history -= 1
                    if current_command_history:
                        fw('\b' * len(string))
                        string = self._command_history[current_command_history]
                        current_autocomplete = self.clear_autocomplete(current_autocomplete)
                        print(string, end = '', flush = True)
                        index = len(string) - 1
            elif char == 'left':
                if index:
                    fw('\b')
                    index -= 1
            elif char == 'right':
                if index < (len(string) - 1):
                    fw(string[index])
                    index += 1
            else:
                col = main_color
                if is_flag and (char == '-'):
                    col = 'white'
                
                if char == ' ':
                    is_flag = False

                if index < (len(string) - 1):
                        self.move(string[index - 1:])
                c.print(f'[{col}]{char}', end = '')

                string = string[:index] + char + string[index:]
                index += 1

        return string

console = ConsoleWrapper()
