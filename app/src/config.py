import os
from .objects import JSONFile

cm_dir: str = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
config_path: str = os.path.join(cm_dir, 'config.json')

class Config(JSONFile):
    """Class representing config."""
    def __init__(self) -> None:
        """Class representing config."""
        super().__init__(config_path)
        
        self.set_value('cm_dir', cm_dir)