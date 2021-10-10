from ..config import Config
from colorama import Fore, Style


def make_bright(color: str) -> str:
        return f'{Style.BRIGHT}{color}'


config = Config()
if config.colorize:
    bright: str = Style.BRIGHT
    reset: str = Style.RESET_ALL
    normal: str = Style.NORMAL
    red: str = Fore.RED
    green: str = Fore.GREEN
    blue: str = Fore.BLUE
    yellow: str = Fore.YELLOW
    gray: str = Style.BRIGHT + Fore.BLACK
    bright_red: str = make_bright(red)
    bright_green: str = make_bright(green)
    bright_blue: str = make_bright(blue)
    bright_yellow: str = make_bright(yellow)
else:
    bright_red = bright_green = bright_blue = bright_yellow = bright = red = green = blue = yellow = reset = normal = ''

