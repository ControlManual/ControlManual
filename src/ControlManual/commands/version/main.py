from typing import List, Dict
from ...client import Client  # Only used for intellisense, will not work if this file is run manually.

HELP: str = 'Get version info.'
USAGE: str = '<version>'
ARGS: dict = {
    'version':
    'Version to get data of, defaults to the current version if left blank.'
}
PACKAGE: str = 'builtin'


async def run(raw: str, args: List[str], kwargs: Dict[str, str],
              flags: List[str], client: Client):

    utils = client.utils
    api = client.api
    errors = client.errors

    if not args:
        version = client.version
    else:
        version = raw

    if not client.connected:
        raise errors.APIError('Not connected to the API.')

    info = api.version_info(version)

    if not info:
        return errors.InvalidArgument('Please specify a valid version.')
    else:
        bool_keys = ["stable", "latest"]
        data = f'Info for version "{version}"\n{utils.reset}{utils.green}'

        for i in bool_keys:
            tmp: bool = info[i]
            data += '\n'
            data += f'{i.capitalize()}: {utils.bright_red}{tmp}{utils.reset}{utils.green}' if not tmp else f'{i.capitalize()}: {utils.bright_green}{tmp}{utils.reset}{utils.green}'

        return utils.success(data)
