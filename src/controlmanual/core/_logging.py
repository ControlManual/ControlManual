import logging
import os
from datetime import datetime

__all__ = ("setup_logging",)


def setup_logging(logs_dir: str) -> None:
    now = datetime.now()
    log_path: str = os.path.join(
        logs_dir, now.strftime("%m.%d.%Y_%H-%M-%S.log")
    )

    logging.basicConfig(
        filename=log_path,
        format="[%(asctime)s] %(levelname)s: %(message)s",
        filemode="w",
        level="DEBUG",
        datefmt="%H:%M:%S",
    )
