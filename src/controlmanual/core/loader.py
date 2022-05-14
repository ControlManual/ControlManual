from ._health import check_health

async def load():
    await check_health()