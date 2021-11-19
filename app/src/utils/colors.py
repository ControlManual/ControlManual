from ..config import Config
from colorama import Style

config = Config()

def rgb(r: int, g: int, b: int) -> str:
    """Function for getting the color code for a given RGB value."""
    return f'\033[38;2;{r};{g};{b}m' if config.colorize else ''


reset: str = Style.RESET_ALL
red: str = rgb(168, 35, 35)
green: str = rgb(40, 168, 35)
blue: str = rgb(35, 66, 168)
yellow: str = rgb(166, 168, 35)
gray: str = rgb(61, 61, 61)
cyan: str = rgb(35, 131, 168)
bright_red: str = rgb(242, 70, 70)
bright_green: str = rgb(98, 217, 13)
bright_blue: str = rgb(38, 111, 255)
bright_yellow: str = rgb(255, 248, 38)
bright_cyan: str = rgb(58, 198, 252)