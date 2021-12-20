import subprocess
import os
from typing import Optional
from ..logger import log

async def run_exe(filename: str, args: Optional[str] = None) -> None:
    """Function for running an executable."""
    await log(f'running executable {filename}')
    with open(os.environ.get('cmpipe'), 'w') as f: # type: ignore
        await log('opened pipe, starting process')
        subprocess.run([filename, args if args else ''], text=True, stdout = f, stderr = f)
    
    await log('executable finished')