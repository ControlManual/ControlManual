from ._health import check_health

async def load():
    """High level function for loading a session."""
    await check_health()