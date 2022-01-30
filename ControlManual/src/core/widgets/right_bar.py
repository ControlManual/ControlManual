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

    def render(self):
        layout = Layout()
        layout.split_column(Panel(f"""User: [important]{self.user}[/important]
OS: [important]{self.system}[/important]
Architecture: [important]{self.machine}[/important]
System Time: [important]{self.sys_time}[/important]
CPU Usage: [important]{self.cpu}%[/important]
Memory: [important]{f"{self.memory.used // 1000000}mB / {self.memory.total // 1000000}mB"}[/important]
Disk: [important]{f"{self.disk.used // 1000000000}gB / {self.disk.total // 1000000000}gB"}[/important]
Computer Name: [important]{platform.node()}[/important]
Uptime: [important]{int(self.uptime) // 60} minutes[/important]""", title = "System Info"), Panel("something here", title = "Some Window"))
        return layout

    def set_vars(self):
        """Update the info."""
        self.cpu = psutil.cpu_percent()
        self.memory = psutil.virtual_memory()
        self.disk = psutil.disk_usage("/")
        self.uptime = time.time() - psutil.boot_time()
        self.sys_time = datetime.datetime.now().strftime('%H:%M:%S')

    def on_mount(self):
        self.set_interval(0.1, self.set_vars)