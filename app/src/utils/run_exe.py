import subprocess
import sys

def run_exe(filename: str, args: str = None) -> str:
    """Function for running an executable."""
    run = subprocess.run([filename, args if args else ''], capture_output=True, text=True, stdout = sys.stdout)

    return run.stdout.strip("\n") if run.stdout else run.stderr.strip("\n")