from typing import Literal

def make_meta(success: bool = True, status: int = 5000, trigger: Literal['generic', 'explicit'] = 'generic', **kwargs) -> dict:
    """Function for generating a metadata dictionary."""
    scolor = 'primary' if status == 5000 else 'danger'
    base = {
        'success': success,
        'status': f'[{scolor}]{status}[/{scolor}]',
        'trigger': trigger
    }

    base.update(kwargs)

    return base
