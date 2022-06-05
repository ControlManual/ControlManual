from dataclasses import dataclass
from typing import List, Dict, Any, Set, Type, AsyncIterator, Union
from .object import Object, String, Integer, Objects, Variable, Function
from .command_errors import ParseError
import re
from inspect import isclass
from .typings import EngineCallable

__all__ = ("parse", "ParsedString")

_PARENS = lambda i: 1 if i == "(" else 0 if not i == ")" else -1
QUOTE_REGEX: str = "('.*')|(\".*\")"
CALL_REGEX: str = r".+\(.*\)"
BUILTINS: Set[Type[Object]] = {String, Integer}


@dataclass
class ParsedString:
    """Class representing a parsed string."""

    command: str
    raw: str
    pos_args: List[Object]
    keyword_args: Dict[str, Object]
    opens_context: bool


async def _split(data: str) -> List[str]:
    split_args: List[str] = []
    buffer: str = ""
    parens: int = 0
    squote = False
    dquote = False

    for i in data:
        if parens == -1:
            raise ParseError('unexpected ")"')

        if not any({parens, squote, dquote}):
            if i == " ":
                split_args.append(buffer)
                buffer = ""
                continue

            if i == ".":
                split_args.append(buffer)
                buffer = ""

        parens += _PARENS(i)
        squote = not squote if i == "'" else squote
        dquote = not dquote if i == '"' else dquote

        buffer += i
    split_args.append(buffer)

    if squote:
        raise ParseError('unexpected "\'"')

    if dquote:
        raise ParseError("unexpected '\"'")

    if parens == 1:
        raise ParseError('expected ")"')

    return split_args


async def _split_params(param_str: str) -> List[str]:
    resp: List[str] = []
    index: int = 0
    new_param: bool = False
    parens: int = 0

    for i in param_str:
        parens += _PARENS(i)

        if len(resp) == index:
            resp.append("")
            new_param = True

        if (i == ",") and (not parens):
            if not resp[index]:
                raise ParseError('unexpected ","')

            index += 1
            continue

        if not (new_param and i == " "):
            resp[index] += i

        if not i == " ":
            new_param = False

    return resp


async def _decode_params(params: List[Object]) -> AsyncIterator[Any]:
    for param in params:
        yield param.raw_data if type(param) in BUILTINS else param


async def _decode_object(obj: Variable) -> Object:
    return await Objects.map_object(
        String(str(obj)) if isclass(obj) else obj
    )  # type: ignore


async def _finalize_params(
    params: List[Object], objects: Objects
) -> List[Object]:
    found = False

    for index, param in enumerate(params):
        raw = param.raw_data

        if isinstance(raw, str) and re.match(CALL_REGEX, raw):
            found = True
            buffer: str = ""
            parsed: List[Union[str, Object]] = []

            for i in raw:
                buffer += i

                if re.match(CALL_REGEX, buffer):
                    parsed.append(
                        buffer
                        if buffer.startswith(".")
                        else await _solve_call(buffer, objects)
                    )
                    buffer = ""

            parsed.append(String(buffer))

            first = parsed[0]
            target = (
                params[index - 1]
                if isinstance(first, str) and first.startswith(".")
                else first
            )

            obj = None

            for string in parsed:
                if isinstance(string, str):
                    obj = (
                        await _solve_object_call(target, string, objects)  # type: ignore
                        if re.match(CALL_REGEX, string)
                        else await objects.lookup_object_attribute(
                            target, string
                        )
                    )

            assert obj is not None
            params[index] = await _decode_object(obj)
            params.pop(index - 1)

        elif isinstance(raw, str) and raw.startswith("."):
            found = True
            params[index] = await objects.lookup_object_attribute(
                params[index - 1], raw
            )

    if found:
        await _finalize_params(params, objects)

    return params


async def _convert_list(data: List[str], objects: Objects) -> List[Object]:
    params: List[Object] = []

    for param in data:
        if re.match(QUOTE_REGEX, param):
            params.append(String(param[1:-1]))
            continue

        if re.match(CALL_REGEX, param):
            params.append(await _solve_call(param, objects))
            continue

        if param.isdigit():
            params.append(Integer(param))  # type: ignore
            continue

        found_obj = objects.objects.get(param)

        if found_obj:
            params.append(await objects.map_object(found_obj))
            continue

        params.append(String(param))

    return await _finalize_params(params, objects)


async def _solve_object_call(
    obj: Object, token: str, objects: Objects
) -> Object:
    name: str = token.split("(")[0]
    params: list = await _split_params(token[token.index("(") + 1 : -1])
    obj_params: List[Object] = await _convert_list(params, objects)

    func: EngineCallable = await objects.ensure_callable(
        await objects.lookup_object_attribute(obj, name)
    )

    return await func.cm_call(
        *[i async for i in _decode_params(obj_params)]
    )  # TODO: is there a way to do this asynchronously?


async def _solve_call(token: str, objects: Objects) -> Object:
    if token.startswith("."):
        return String(token)

    name: str = token.split("(")[0]
    params: list = await _split_params(token[token.index("(") + 1 : -1])
    obj_params: List[Object] = await _convert_list(params, objects)
    obj = await objects.lookup(name, ensure_callable=True)

    return obj(i async for i in _decode_params(obj_params))


async def _handle_parse(data: str, objects: Objects) -> ParsedString:
    raw_split: List[str] = data.split(" ", maxsplit=1)
    command: str = raw_split.pop(0)
    split_args = await _split(raw_split.pop(0)) if raw_split else []

    return ParsedString(
        command,
        data,
        await _convert_list(split_args, objects),
        {},
        data.endswith("{"),
    )


async def parse(data: str, objects: Objects) -> AsyncIterator[ParsedString]:
    """Parse a string of data."""
    for i in re.split("&&| && ", data):
        yield await _handle_parse(i, objects)
