from .utils.colors import *
from .static import static_error
from typing import Callable
import os

current_theme: str = config.theme
"""
Themes should follow this format below:

{
    <theme>: {
        'color': [<primary>, <secondary>, <important>, <error>, <warning>],
        'functions': {}
    }
}
"""

def default_input_string(colors: dict, client) -> str:
    return f'{client._path} {colors["primary"]}{client.config.input_sep}{reset} '

def crystal_input_string(colors: dict, client) -> str:
    slash = '\\' if os.name == 'nt' else '/'
    path = f'{cyan}{slash}{bright_cyan}'.join(client.path_str.split(slash))
    
    return f'{path} {cyan}{client.config.input_sep}{reset} '

THEMES: dict = {
    'default': {
        'colors': [rgb(105, 155, 255), rgb(64, 112, 207), reset, rgb(232, 73, 65), reset],
        'functions': {
            'input_string': default_input_string,
        }
    },
    'crystal': {
        'colors': [bright_cyan, cyan, reset, rgb(176, 255, 64), reset],
        'functions': {
            'input_string': crystal_input_string,
        }
    },
    'shadow': {
        'colors': [reset, rgb(240, 240, 240), reset, rgb(242, 213, 213), rgb(247, 247, 210)],
        'functions': {
            'input_string': default_input_string,
        }
    },
    'bash': {
        'colors': [bright_green, green, bright_blue, bright_red, bright_yellow],
        'functions': {
            'input_string': default_input_string
        }
    }
}

colors = THEMES.get(current_theme)

if not colors:
    static_error('invalid theme')

primary, secondary, important, danger, warning = colors['colors']
theme_dictionary: dict = {
    "primary": primary,
    "secondary": secondary,
    "important": important,
    "danger": danger,
    "warning": warning,
    "reset": reset
}


def input_string(client) -> str:
    resolver: Callable = colors['functions'].get('input_string')
    return resolver(theme_dictionary, client) if resolver else ''