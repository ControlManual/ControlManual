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

        if (i == " ") and not any({parens, squote, dquote}):
            split_args.append(buffer)
            buffer = ''
            continue
        
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

def _convert_list(data: List[str], objects: Objects) -> List[Object]:
    params: List[Object] = []

    for param in data:
        if ').' in param:
            buffer: str = ''
            parsed: List[Union[str, Object]] = []

            for i in param:
                buffer += i

                if re.match(CALL_REGEX, buffer):
                    parsed.append(buffer if buffer.startswith('.') else _solve_call(buffer, objects))
                    buffer = ''

            parsed.append(String(buffer))

            obj: Variable = parsed.pop(0)  # type: ignore

            for string in parsed:
                if isinstance(string, str):
                    obj = _solve_object_call(
                        obj,  # type: ignore
                        string,
                        objects
                    ) \
                    if re.match(CALL_REGEX, string) \
                    else objects.lookup_object_attribute(
                        obj,
                        string
                    )

            params.append(_decode_object(obj))
            continue


        if re.match(QUOTE_REGEX, param):
            params.append(String(param[1:-1]))
            continue

        if re.match(CALL_REGEX, param):
            params.append(_solve_call(param, objects))
            continue
        
        if param.isdigit():
            params.append(Integer(param))  # type: ignore
            continue    

        params.append(String(param))

    return params

def _solve_object_call(obj: Object, token: str, objects: Objects) -> Object:
    name: str = token.split('(')[0]
    params: list = _split_params(token[token.index('(') + 1:-1])
    obj_params: List[Object] = _convert_list(params, objects)

    func: Function = objects.ensure_callable(objects.lookup_object_attribute(obj, name))

    return func.call(*_decode_params(obj_params))

def _solve_call(token: str, objects: Objects) -> Object:
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