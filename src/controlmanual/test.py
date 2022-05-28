from core._parse import parse
from core.object import String, Integer, Objects


objects = Objects({'String': String, 'Integer': Integer})
for i in parse("echo String('hello world').upper()", objects):
    print(i)