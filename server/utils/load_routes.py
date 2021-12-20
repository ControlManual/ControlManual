import os
import importlib
from fastapi import FastAPI
from colorama import Fore, Style

directory: str = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def load_routes(app: FastAPI) -> None:
    """Function for loading routes into an app."""
    for root, _, files in os.walk(os.path.join(directory, "routes")):
        for file in files:
            if file.endswith(".pyc"):
                continue

            current_path = os.path.join(root, file)
            spec = importlib.util.spec_from_file_location(file[:-3], current_path)
            mod = importlib.util.module_from_spec(spec)
            spec.loader.exec_module(mod)
            if hasattr(mod, "error"):
                if not hasattr(mod, "handler"):
                    print(
                        f'{Fore.RED}{Style.BRIGHT}Failed to load error handler "{file}"{Style.RESET_ALL}'
                    )
                else:
                    print(
                        f'{Fore.GREEN}{Style.BRIGHT}Successfully loaded error handler "{file}"{Style.RESET_ALL}'
                    )
                app.add_exception_handler(mod.error, mod.handler)
            else:
                if (not hasattr(mod, "router")) or (not hasattr(mod, "prefix")):
                    print(
                        f'{Fore.RED}{Style.BRIGHT}Failed to load route "{file}"{Style.RESET_ALL}'
                    )
                else:
                    app.include_router(mod.router, prefix=mod.prefix)
                print(
                    f'{Fore.GREEN}{Style.BRIGHT}Successfully loaded route "{file}"{Style.RESET_ALL}'
                )
