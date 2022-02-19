import datetime
import getpass
import platform
import time

import distro
import psutil
from cpuinfo import get_cpu_info
from rich import box
from rich.layout import Layout
from rich.panel import Panel
from rich.table import Table
from textual.reactive import Reactive
from textual.widget import Widget
from ..config import config
from rich.console import Console, ConsoleOptions, RenderResult
import logging
import pyfiglet
from rich.align import Align

from ...utils import not_null

__all__ = ["RightBar"]

lower = lambda x: x.lower() if config['lowercase'] else x

class ProcPanel:
    def __init__(self) -> None:
        self.base = lambda g: f'rgb(53,{g},42)'
        self.base_num: int = 179
        self.a = lambda x: f'[{self.base(self.base_num)}]{x}[/{self.base(self.base_num)}]'
        self.current: int = 3
    
    def __rich_console__(self, console: Console, options: ConsoleOptions) -> RenderResult:
        self.table = Table(box=box.SIMPLE)

        for i in ("Process", "PID", "Memory", "User"):
            self.table.add_column(lower(i))

        a = self.a

        # process_iter cannot be put into a list, it must be an iterator
        for proc in psutil.process_iter():
            self.table.add_row(
                a(proc.name()),
                a(str(proc.pid)),
                a(str(round(proc.memory_percent(), 2))),
                a(proc.username()),
            )

            self.current += 1

            if (options.max_height - self.current) <= 3:
                self.base_num -= 60

        yield Panel(self.table, title="Processes")

class RightBar(Widget):
    """Display system info."""

    machine = platform.machine()
    cpu = Reactive(psutil.cpu_percent())
    memory = Reactive(psutil.virtual_memory())
    disk = Reactive(psutil.disk_usage("/"))
    system = (
        distro.name(pretty=True)
        if platform.system() == "Linux"
        else f"{platform.system()} {platform.release()} {platform.version()}"
    )
    uptime = Reactive(time.time() - psutil.boot_time())
    sys_time = Reactive(datetime.datetime.now().strftime("%H:%M:%S"))
    round_time = Reactive(datetime.datetime.now().strftime("%H:%M"))
    user = getpass.getuser()
    node = platform.node()
    battery = Reactive(psutil.sensors_battery())  # type: ignore
    cpu_info = get_cpu_info()

    def render(self):
        layout = Layout()
        
        layout.split_column(
            Panel(
                f"""[success]{self.user}[/success]@[secondary]{self.node}[/secondary]
{'-' * len(f'{self.user}@{self.node}')}

[important]{lower("OS")}[/important]: {self.system}
[important]{lower("Architecture")}[/important]: {self.machine}
[important]{lower("System Time")}[/important]: {self.sys_time}
[important]{lower("CPU")}[/important]: {self.cpu_info["brand_raw"]}
[important]{lower("Battery")}[/important]: {round(not_null(self.battery).percent, 2)}% ({"not " if not not_null(self.battery).power_plugged else ""}plugged in)
[important]{lower("CPU Usage")}[/important]: {self.cpu}%
[important]{lower("Memory")}[/important]: {f"{self.memory.used // 1000000}mB / {self.memory.total // 1000000}mB"}
[important]{lower("Disk")}[/important]: {f"{self.disk.used // 1000000000}gB / {self.disk.total // 1000000000}gB"}
[important]{lower("Uptime")}[/important]: {int(self.uptime) // 60} minutes""",
                title="System Info",
            ),
            ProcPanel(),
            Panel(Align.center(pyfiglet.figlet_format(self.round_time, font = "big", width = 30), vertical = "middle"), title = "Time")
        )
        return layout

    def set_vars(self):
        """Update the info."""
        self.cpu = psutil.cpu_percent()
        self.memory = psutil.virtual_memory()
        self.disk = psutil.disk_usage("/")
        self.uptime = time.time() - psutil.boot_time()
        self.sys_time = datetime.datetime.now().strftime("%H:%M:%S")
        self.battery = psutil.sensors_battery()  # type: ignore
        self.round_time = datetime.datetime.now().strftime("%H:%M")

    def on_mount(self):
        self.set_interval(0.1, self.set_vars)
