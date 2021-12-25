import py7zr
import aiohttp
import aiofiles
import os
from ..constants import cm_dir

async def download_package(package: str) -> bool:
    target: str = os.path.join(cm_dir, 'commands')
    file: str = os.path.join(target, f'temp_package_{package}.7z')

    async with aiohttp.ClientSession() as session:
        async with session.get('https://api.controlmanual.xyz/package/get', params = {'package': package}) as resp:
            if not resp.status == 200:
                return False

            async with aiofiles.open(file, 'wb') as f:
                await f.write(await resp.content.read())

    with py7zr.SevenZipFile(file, 'r') as archive:
        archive.extractall(target)
    
    os.remove(file)
    return True
