import sys


def title(text: str) -> None:
    """Function for changing terminal title."""
    sys.stdout.write(f"\x1b]2;{text}\x07")
