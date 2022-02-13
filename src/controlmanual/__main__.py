from .source import main_wrap
import sys
import os

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "source"))

if __name__ == '__main__':
    main_wrap()