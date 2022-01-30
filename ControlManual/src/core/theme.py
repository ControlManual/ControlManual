from rich.theme import Theme
from rich.console import Console

theme = Theme({
    "error": "bold red",
    "important": "bold green",
    "success": "green"
})
console_object = Console(theme = theme)