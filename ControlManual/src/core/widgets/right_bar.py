from textual.reactive import Reactive
from textual.widget import Widget
from rich.panel import Panel
import time
import psutil
import distro
import platform
import getpass
import datetime
from rich.layout import Layout
from cpuinfo import get_cpu_info
from rich.table import Table
from rich import box

__all__ = ["RightBar"]

class RightBar(Widget):
    """Display system info."""
    machine = platform.machine()
    cpu = Reactive(psutil.cpu_percent())
    memory = Reactive(psutil.virtual_memory())
    disk = Reactive(psutil.disk_usage("/"))
    system = (distro.name(pretty=True) if platform.system() == "Linux" else f"{platform.system()} {platform.release()} {platform.version()}")
    uptime = Reactive(time.time() - psutil.boot_time())
    sys_time = Reactive(datetime.datetime.now().strftime('%H:%M:%S'))
    user = getpass.getuser()
    node = platform.node()
    battery = Reactive(psutil.sensors_battery()) # type: ignore
    cpu_info = get_cpu_info()

    def render(self):
        layout = Layout()
        table = Table(box = box.SIMPLE)

        table.add_column("Process")
        table.add_column("PID")
        table.add_column("Memory")
        table.add_column("User")

        a = lambda x: f"[important]{x}[/important]"

        for proc in psutil.process_iter():
            table.add_row(a(proc.name()), a(str(proc.pid)), a(str(round(proc.memory_percent(), 2))), a(proc.username()))


        layout.split_column(Panel(f"""[success]{self.user}[/success]@[important]{self.node}[/important]
{'-' * len(f'{self.user}@{self.node}')}

[important]OS[/important]: {self.system}
[important]Architecture[/important]: {self.machine}
[important]System Time[/important]: {self.sys_time}
[important]CPU[/important]: {self.cpu_info["brand_raw"]}
[important]Battery[/important]: {round(self.battery.percent, 2)}% ({"not " if not self.battery.power_plugged else ""}plugged in)
[important]CPU Usage[/important]: {self.cpu}%
[important]Memory[/important]: {f"{self.memory.used // 1000000}mB / {self.memory.total // 1000000}mB"}
[important]Disk[/important]: {f"{self.disk.used // 1000000000}gB / {self.disk.total // 1000000000}gB"}
[important]Uptime[/important]: {int(self.uptime) // 60} minutes""", title = "System Info"), Panel(table, title = "Processes"))
        return layout

    def set_vars(self):
        """Update the info."""
        self.cpu = psutil.cpu_percent()
        self.memory = psutil.virtual_memory()
        self.disk = psutil.disk_usage("/")
        self.uptime = time.time() - psutil.boot_time()
        self.sys_time = datetime.datetime.now().strftime('%H:%M:%S')
        self.battery = psutil.sensors_battery() # type: ignore

    def on_mount(self):
        self.set_interval(0.1, self.set_vars)