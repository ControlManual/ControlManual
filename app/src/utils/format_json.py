import json
from .colors import *

def format_json(raw: str) -> str:
    formatted: str = json.dumps(raw, indent=4)
    formatted_color: str = reset
    temp: bool = False

    for i in formatted:
        if i == '"':
            if temp:
                formatted_color += '"' + reset
                temp = False
            else:
                temp = True
                formatted_color += green + '"'
        else:
            formatted_color += i
    
    formatted_color = formatted_color.replace('true', f'{bright_blue}true{reset}')\
        .replace('false', f'{bright_red}false{reset}')

    return formatted_color