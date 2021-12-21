from typing import List, Dict
from ...client import Client  # Only used for intellisense, will not work if this file is run manually.
import sys

HELP: str = 'Exit the instance.'
USAGE: str = '<code>'
ARGS: dict = {'code': 'Exit code to use.'}
ARGS_HELP: dict = {
    'code': {
        'type': 'Number',
        'when_unspecified': 'Uses exit code 0.'
    }
}
PACKAGE: str = 'builtin'


async def run(raw: str, args: List[str], kwargs: Dict[str, str],
              flags: List[str], client: Client):
    errors = client.errors

    if not args:
        code: int = 0
    else:
        try:
            code: int = int(args[0])
        except ValueError:
            raise errors.InvalidArgument('Please enter a valid status code.')

    client._thread_running = True  # type: ignore
    sys.exit(code)
