import os

cm_dir: str = os.path.dirname(
    os.path.abspath(__file__))  # for prevention of circular dependency

if __name__ == "__main__":
    print(cm_dir)
