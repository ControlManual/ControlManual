import sys

sys.path.insert(0, './src/')

import ControlManual

try:
    ControlManual.main_sync()  # type: ignore
except KeyboardInterrupt:
    sys.exit(0)
