import os
from typing import Literal
import re
import string

def err(message: str, path: str, lineno: int) -> Literal[False]:
    print(f"{path}:{lineno} - {message}")
    return False

def _parse_text(path: str, code: str) -> bool:
    result = True

    lines = code.split("\n")
    for index, i in enumerate(lines):
        if len(i) > 80:
            err("line too long", path, index + 1)
            result = False
        if i.startswith("\t"):
            err("used tab instead of spaces", path, index + 1)
            result = False

        indent = 0

        for x in i:
            if x == ' ':
                indent += 1
            else:
                break
        
        if indent % 4 != 0:
            err("indents should use 4 spaces each", path, index + 1)
            result = False
        
        if (indent == len(i)) and (indent):
            err("empty line contains whitespace", path, index + 1)
            result = False
    
    return result

def main():
    result = True

    for root, _, files in os.walk("./src"):
        for name in files:
            path = os.path.join(root, name)
            with open(path, "r") as f:
                if not _parse_text(path, f.read()):
                    result = False
    
    if not result:
        exit(1)
    
    print("OK")

if __name__ == '__main__':
    main()