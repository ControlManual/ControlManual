def error(message: str) -> None:
    raise Exception("due to limitations with asyncio, this function no longer works. please use Client.error or ConsoleWrapper.error instead.")