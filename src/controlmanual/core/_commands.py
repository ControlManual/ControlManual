from .command import Command
from typing import Dict, Any
import os
import importlib.util
import logging
from types import ModuleType
from .command_errors import CommandLoadFailed


async def _load_module(mod_name: str, mod_path: str) -> ModuleType:
    spec = importlib.util.spec_from_file_location(mod_name, mod_path)

    if not spec:
        logging.critical(
            f'failed to load spec for "{mod_name}" at "{mod_path}"'
        )
        raise RuntimeError("failed to load spec")

    mod = importlib.util.module_from_spec(spec)
    loader = spec.loader

    if not loader:
        logging.critical(f'spec for "{mod_name}" at "{mod_path}" has no loader')
        raise RuntimeError("spec has no loader")

    try:
        loader.exec_module(mod)
    except Exception as e:
        logging.error(
            f'failed to execute module "{mod_name}" at "{mod_path}": {e}'
        )
        raise CommandLoadFailed(f'failed to load module "{mod_name}"') from e

    return mod


async def _extract_values(mod_name: str, obj: Any) -> Command:
    invalid_schema_msg: str = f'"{mod_name}" has an invalid argument schema'
    name = getattr(obj, "__name__", None) or mod_name
    runner = getattr(obj, "runner" if hasattr(obj, "runner") else name, None)
    description = str(
        getattr(obj, "description", None) or "No description specified."
    )
    schema = getattr(obj, "schema", None) or {}

    if not runner:
        logging.error(
            f'failed to extract value for command: "{mod_name}" (no runner found)'
        )
        raise CommandLoadFailed(f'"{mod_name}" has no runner')

    if not isinstance(schema, dict):
        logging.error(f'invalid schema passed for command "{mod_name}"')
        raise CommandLoadFailed(invalid_schema_msg)

    for key in schema:
        if not isinstance(schema, dict):
            logging.error(
                f'invalid argument passed for schema inside "{mod_name}"'
            )
            raise CommandLoadFailed(invalid_schema_msg)

        values = (
            key.get("required"),
            key.get("type"),
        )

        if not all(values):
            not_found = "required" if not values[0] else "type"
            raise CommandLoadFailed(
                f'invalid schema in "{mod_name}": "{key}" is missing key "{not_found}"'
            )

        if not type(values[1]) is type:
            logging.error(
                f'invalid argument passed for schema inside "{mod_name}": "{values[1]}" is not a valid type inside "{key}"'
            )
            raise CommandLoadFailed(
                f'invalid type passed for "{key}" in "{mod_name}"'
            )

    return Command(runner, description, schema)


async def load_commands(path: str) -> Dict[str, Command]:
    commands: Dict[str, Command] = {}

    for root, dirs, files in os.walk(path):
        for i in files:
            name: str = i.split(".")[0]

            if i.endswith(".py"):
                commands[name] = await _extract_values(
                    name, await _load_module(name, os.path.join(path, i))
                )

    return commands
