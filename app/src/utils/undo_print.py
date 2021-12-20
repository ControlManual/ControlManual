import sys


def undo_print() -> None:
    sys.stdout.write("\033[K")
