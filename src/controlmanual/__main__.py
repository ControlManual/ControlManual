import os
import sys

from .source.core.health import check_health

check_health()
sys.path.insert(0, os.path.join(os.path.dirname(__file__), "source"))

if __name__ == "__main__":
    from .source.main import main_wrap
    main_wrap()
