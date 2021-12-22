import aiohttp


async def is_online() -> bool:
    """Function for checking if the api is online."""
    try:
        async with aiohttp.ClientSession() as session:
            async with session.get('https://api.github.com') as response:
                if not response.status == 200:
                    raise Exception()

        return True
    except:
        return False
