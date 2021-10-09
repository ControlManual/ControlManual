import os

def clear() -> None:
    """Cross platform function for clearing terminal window."""
    if os.name == 'nt': # if the platform is windows
        print(chr(27) + "[2J", end="") # clears screen for windows
    else: # for anything other than windows
        print("\033c", end="") # clears screen for linux and mac
