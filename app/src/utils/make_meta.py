from typing import Literal

def make_meta(success: bool = True, status: int = 5000, **kwargs) -> dict:
    """Function for generating a metadata dictionary."""
    scolor = 'primary' if status == 5000 else 'danger'
    s = '[primary]true[/primary]' if success else '[danger]false[/danger]'
    base = {
        'success': s,
        'status': f'[{scolor}]{status}[/{scolor}]'
    }

    base.update(kwargs)

    return base
