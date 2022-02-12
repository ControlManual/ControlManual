from rich.theme import Theme
from rich.console import Console
from .config import config

tc = config['truecolor']

theme = Theme({
    "error": "bold #ef4a4a" if tc else 'bold red',
    "important": "bold #1a9628" if tc else 'bold green',
    "success": "#1a9628" if tc else 'green'
})
console_object = Console(theme = theme, color_system = 'truecolor' if tc else 'standard')