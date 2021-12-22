import py7zr
import aiohttp
import aiofiles
from ..console import console

async def download_package(package: str, file: str, target: str) -> bool:
    async with aiohttp.ClientSession() as session:
        async with session.get('http://localhost:5000/package/get', params = {'package': package}) as resp:
            if not resp.status == 200:
                return False

            async with aiofiles.open(file, 'wb') as f:
                await f.write(await resp.content.read())

    with py7zr.SevenZipFile(file, 'r') as archive:
        archive.extractall(target)

    return True
