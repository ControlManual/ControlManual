from typing import Iterable


class Vector[T]:
    def __init__(self, /, __iter: Iterable[T] | None = ...) -> None:
        ...

    def get(self, /, __index: int) -> T:
        ...

    def append(self, /, __item: T) -> None:
        ...

    def insert(self, /, __index: int, __item: T) -> None:
        ...

    @property
    def length(self) -> int:
        ...

class Map[T]:
    def __init__(self, /, __inital_size: int) -> None:
        ...

    def set(self, /, __key: str, __value: T) -> None:
        ...

    def get(self, /, __key: str) -> T:
        ...
    
    @property
    def capacity(self) -> int:
        ...

    @property
    def length(self) -> int:
        ...
