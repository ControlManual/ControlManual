from dataclasses import dataclass
from typing import List, Dict, Any, Set, Type, Iterator, Union
from .object import Object, String, Integer, Objects, Variable, Function
from .command_errors import ParseError
import re
from inspect import isclass

__all__ =  (
    "parse",
    "ParsedString"
)

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

def _split(data: str) -> List[str]:
    split_args: List[str] = []
    buffer: str = ''
    parens: int = 0
    squote = False
    dquote = False
    
    for i in data:
        if parens == -1:
            raise ParseError('unexpected ")"')

        if not any({parens, squote, dquote}):
            if (i == " "):
                split_args.append(buffer)
                buffer = ''
                continue

            if i == ".":
                split_args.append(buffer)
                buffer = ''
        
        parens += _PARENS(i)
        squote = not squote if i == "'" else squote
        dquote = not dquote if i == '"' else dquote
  
        buffer += i
    split_args.append(buffer)

    if squote:
        raise ParseError("unexpected \"'\"")

    if dquote:
        raise ParseError("unexpected '\"'")

    if parens == 1:
        raise ParseError('expected ")"')

    return split_args

def _split_params(param_str: str) -> List[str]:
    resp: List[str] = []
    index: int = 0
    new_param: bool = False
    parens: int = 0

    for i in param_str:
        parens += _PARENS(i)

        if len(resp) == index:
            resp.append('')
            new_param = True
        
        if (i == ',') and (not parens):
            if not resp[index]:
                raise ParseError('unexpected ","')

            index += 1
            continue
        
        if not (new_param and i == ' '):
            resp[index] += i

        if not i == ' ':
            new_param = False

    return resp

def _decode_params(params: List[Object]) -> Iterator[Any]:
    for param in params:
        yield param.raw_data if type(param) in BUILTINS else param

def _decode_object(obj: Variable) -> Object:
    return Objects.ensure_object(String(str(obj)) if isclass(obj) else obj)  # type: ignore

def _finalize_params(params: List[Object], objects: Objects) -> List[Object]:
    found = False
    for index, param in enumerate(params):
        raw = param.raw_data

        if isinstance(raw, str) and re.match(CALL_REGEX, raw):
            found = True
            buffer: str = ''
            parsed: List[Union[str, Object]] = []

            for i in raw:
                buffer += i

                if re.match(CALL_REGEX, buffer):
                    parsed.append(buffer if buffer.startswith('.') else _solve_call(buffer, objects))
                    buffer = ''

            parsed.append(String(buffer))

            first = parsed[0]
            target = params[index - 1] if isinstance(first, str) and first.startswith('.') else first

            obj = None

            for string in parsed:
                if isinstance(string, str):
                    obj = _solve_object_call(
                        target,  # type: ignore
                        string,
                        objects
                    ) \
                    if re.match(CALL_REGEX, string) \
                    else objects.lookup_object_attribute(
                        target,
                        string
                    )

            assert obj
            params[index] = _decode_object(obj)
            params.pop(index - 1)

        elif isinstance(raw, str) and raw.startswith('.'):
            found = True
            params[index] = objects.lookup_object_attribute(params[index - 1], raw)  # type: ignore
            params.pop(index - 1)

    if found:
        _finalize_params(params, objects)

    return params

def _convert_list(data: List[str], objects: Objects) -> List[Object]:
    params: List[Object] = []

    for param in data:
        if re.match(QUOTE_REGEX, param):
            params.append(String(param[1:-1]))
            continue

        if re.match(CALL_REGEX, param):
            params.append(_solve_call(param, objects))
            continue
        
        if param.isdigit():
            params.append(Integer(param))  # type: ignore
            continue
        
        found_obj = objects.objects.get(param)
        if found_obj:
            params.append(found_obj)
            continue

        params.append(String(param))

    return _finalize_params(params, objects)

def _solve_object_call(obj: Object, token: str, objects: Objects) -> Object:
    name: str = token.split('(')[0]
    params: list = _split_params(token[token.index('(') + 1:-1])
    obj_params: List[Object] = _convert_list(params, objects)

    func: Function = objects.ensure_callable(objects.lookup_object_attribute(obj, name))

    return func.call(*_decode_params(obj_params))

def _solve_call(token: str, objects: Objects) -> Object:
    if token.startswith('.'):
        return String(token)

    name: str = token.split('(')[0]
    params: list = _split_params(token[token.index('(') + 1:-1])
    obj_params: List[Object] = _convert_list(params, objects)
    obj = objects.lookup(name)

    return obj(*_decode_params(obj_params))

def _handle_parse(data: str, objects: Objects) -> ParsedString:
    raw_split: List[str] = data.split(" ", maxsplit=1)
    command: str = raw_split.pop(0)
    split_args = _split(raw_split.pop(0)) if raw_split else []
    
    return ParsedString(
        command,
        data,
        _convert_list(split_args, objects),
        {},
        data.endswith('{')
    )

def parse(data: str, objects: Objects) -> Iterator[ParsedString]:
    """Parse a string of data."""
    for i in re.split('&&| && ', data):
        yield _handle_parse(i, objects)