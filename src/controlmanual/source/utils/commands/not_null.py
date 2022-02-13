from typing import Optional, TypeVar

T = TypeVar("T")


def not_null(data: Optional[T]) -> T:
    assert (
        data is not None
    )  # not checking using not since things like 0 and "" evaluate to false
    return data
