from typing import List, Dict
from ...client import Client  # Only used for intellisense, will not work if this file is run manually.

HELP: str = 'Compare 2 items.'
USAGE: str = '<item_1> <comparison> <item_2> <true> <false> [flags]'
ARGS: dict = {
    'item_1': 'Item to compare with item 2.',
    'comparison': 'Operator to use for comparison.',
    'item_2': 'Item to compare with item 1.',
    'true': 'Command to run if the comparison is true.',
    'false': 'Command to run if the comparison is false.'
}
ARGS_HELP: dict = {
    'item_1': {
        'type': 'String',
        'when_flag_is_passed': [['int', 'Argument type becomes "Number"']]
    },
    'item_2': {
        'type': 'String',
        'when_flag_is_passed': [['int', 'Argument type becomes "Number"']]
    },
    'comparison': {
        'valid_values': ['==', '=', 'is', 'in'],
        'when_flag_is_passed':
        [['int', 'Only values of "<", "=", "==", and ">" are allowed.']]
    },
    'true': {
        'type': 'Command'
    },
    'false': {
        'type': 'Command'
    },
}
FLAGS: dict = {'int': 'Compare item 1 and 2 numerically.'}
PACKAGE: str = 'builtin'


async def run(raw: str, args: List[str], kwargs: Dict[str, str],
              flags: List[str], client: Client):

    errors = client.errors
    utils = client.utils

    if len(args) < 5:
        raise errors.NotEnoughArguments('Invalid arguments.')

    item1, operator, item2, true, false = args
    use_int: bool = 'int' in flags

    if operator in ['==', '=', 'is', 'in'
                    ] if not use_int else ['<', '=', '==', '>']:
        if not use_int:
            comparisons = {'is': item1 == item2, 'in': item1 in item2}

            comparison = comparisons[operator if not operator == '=' else '==']

            return client.run_command(true if comparison else false)
        else:
            try:
                item1 = int(item1)
                item2 = int(item2)
            except ValueError:
                raise errors.InvalidArgument('Please specify a valid number.')

            comparisons = {
                '<': item1 < item2,
                '>': item1 > item2,
                '==': item1 == item2
            }
            comparison = comparisons[operator if not operator == '=' else '==']
            return client.run_command(true if comparison else false)
    else:
        raise errors.InvalidArgument(
            'Please specify a valid comparison operator.')
