import sys

sys.path.insert(0, './ControlManual/')

import src # type: ignore

try:
    src.main_sync()
except KeyboardInterrupt:
    sys.exit(0)
