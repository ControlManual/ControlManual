import sys

sys.path.insert(0, './ControlManual/')

import src

try:
    src.main_sync()  # type: ignore
except KeyboardInterrupt:
    sys.exit(0)
