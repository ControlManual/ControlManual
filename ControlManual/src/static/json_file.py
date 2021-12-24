import json
from typing import Dict, Any, List
import os
from .static_error import static_error


class JSONFile:
    """Class representing a JSON file."""

    def __init__(
        self,
        file_path: str,
        required: List[str] = [],
        require_keys_under: Dict[str, List[str]] = {},
    ) -> None:
        """Class representing a JSON file."""
        if not os.path.exists(file_path):
            static_error(f"{file_path} does not exist.")

        with open(file_path) as f:
            try:
                raw = json.load(f)
            except json.JSONDecodeError:
                static_error(f'failed to parse JSON file "{file_path}"')

        self._raw = raw

        for i in raw:
            setattr(self, i, raw[i])

        self._path = file_path

        for i in required:
            if i not in self._raw:
                static_error(f'key "{i}" is required in "{file_path}".')

        for key, value in require_keys_under.items():
            for i in value:
                if i not in self._raw[key]:
                    static_error(
                        f'key "{i}" is required in the "{file_path}" key "{key}".'
                    )

    @property
    def path(self) -> str:
        """Location of the JSON file."""
        return self._path

    @path.setter
    def set_path(self, value: str) -> None:
        self._path = value

    @property
    def raw(self) -> Dict[Any, Any]:
        """Raw json dictionary."""
        return self._raw

    def set_value(self, key: str, value: Any) -> None:
        """Set the value of a key in the file."""
        setattr(self, key, value)
        self._raw[key] = value

        self.update_values()

    def erase_value(self, key: str) -> None:
        """Remove a key from the file."""
        delattr(self, key)
        del self._raw[key]

        self.update_values()

    def update_values(self) -> None:
        """Match keys with file."""

        with open(self.path, "w") as f:
            json.dump(self._raw, f, indent=4)
