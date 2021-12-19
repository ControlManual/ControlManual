import subprocess
import os
from typing import Optional
from ..logger import log

def run_exe(filename: str, args: Optional[str] = None) -> None:
    """Function for running an executable."""
    log(f'running executable {filename}')
    with open(os.environ.get('cmpipe'), 'w') as f: # type: ignore
        log('opened pipe, starting process')
        subprocess.run([filename, args if args else ''], text=True, stdout = f, stderr = f)
    
    log('executable finished')