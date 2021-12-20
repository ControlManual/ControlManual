import sys


def static_error(message: str, prefix: str = "ERROR: "):
    """Function for closing the program with an error message without being dependent on any part of the project."""
    print(f"{prefix}{message}")
    sys.exit(1)
