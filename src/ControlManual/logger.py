from datetime import datetime
from types import FrameType
from .cm_dir import cm_dir
from .check_health import check_health
import os
import inspect
import aiofiles
import asyncio

asyncio.run(check_health())
now = datetime.now()

log_path: str = os.path.join(cm_dir, "logs",
                             now.strftime("%m.%d.%Y_%H-%M-%S.log"))
open(log_path, "w").close()
os.environ["cmlog_buffer"] = ""


async def log(*args) -> None:
    """Function for writing to the log file."""

    now = datetime.now()
    frame: FrameType = inspect.currentframe().f_back  # type: ignore

    os.environ["cmlog_buffer"] += now.strftime(
        f"[%H:%M:%S] {frame.f_code.co_name} at {os.path.basename(frame.f_code.co_filename)}:{frame.f_lineno} - {' '.join(args)}\n"
    )


async def flush() -> None:
    async with aiofiles.open(log_path, "a") as f:
        await f.write(os.environ["cmlog_buffer"])

    os.environ["cmlog_buffer"] = ""
