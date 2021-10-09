import json
from typing import Dict, Any

class JSONFile:
    """Class representing a JSON file."""
    def __init__(self, file_path: str) -> None:
        """Class representing a JSON file."""
        with open(file_path) as f:
            raw = json.load(f)

        self._raw = raw

        for i in raw:
            setattr(self, i, raw[i])
        
        self._path = file_path

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

        with open(self.path, 'w') as f:
            json.dump(self._raw, f, indent = 4)
