import subprocess
from typing import Optional
from ..logger import log
import tempfile
from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler
from ..console import console
import rethread
import time
import os

END: bool = False


class Handler(FileSystemEventHandler):
    @classmethod
    def on_any_event(cls, event):
        if event.event_type == "modified":
            if not os.path.exists(PIPE_FILE.name):
                return

            with open(PIPE_FILE.name, "r") as f:  # type: ignore
                console.write(f.read())


class OnMyWatch:
    def __init__(self):
        self.observer = Observer()

    def run(self):
        event_handler = Handler()
        self.observer.schedule(event_handler, PIPE_FILE.name, recursive=True)
        self.observer.start()
        try:
            while True:
                if END:
                    break
                time.sleep(0.1)
        except:
            self.observer.stop()

        self.observer.join()


def watch_pipe():
    watch = OnMyWatch()
    watch.run()


async def run_exe(filename: str, args: Optional[str] = None) -> None:
    """Function for running an executable."""
    await log(f"preparing to run executable {filename}")

    global PIPE_FILE
    PIPE_FILE = tempfile.NamedTemporaryFile(prefix="controlmanual_",
                                            suffix="_pipe")

    await log("created pipe, starting process")

    rethread.thread(watch_pipe)
    with open(PIPE_FILE.name, "w") as f:
        subprocess.run([filename, args if args else ""],
                       text=True,
                       stdout=f,
                       stderr=f)
    global END
    END = True

    PIPE_FILE.close()

    await log("executable finished")
