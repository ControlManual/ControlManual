from typing import List, Dict
from ...client import Client  # Only used for intellisense, will not work if this file is run manually.
import requests

HELP: str = 'Send a HTTP request.'
USAGE: str = '<method> <url> [kwargs] [flags]'
ARGS: dict = {
    'method': 'HTTP method to use.',
    'url': 'URL to send request.',
    'kwargs': 'HTTP arguments to send with request.'
}
ARGS_HELP: dict = {
    'method': {
        'valid_values': ['get', 'post', 'put', 'patch', 'delete'],
    }
}
FLAGS: dict = {
    'json': 'Show JSON response.',
    'text': 'Show text response.',
    'headers': 'Show response headers.'
}
PACKAGE: str = 'builtin'


async def run(raw: str, args: List[str], kwargs: Dict[str, str],
              flags: List[str], client: Client):
    """
    utils = client.utils
    errors = client.errors

    if len(args) < 2:
        raise errors.NotEnoughArguments('Please specify a method and url.')
    
    methods: dict = {
        'get': requests.get,
        'post': requests.post,
        'put': requests.put,
        'patch': requests.patch,
        'delete': requests.delete,
        'options': requests.options
    }

    if args[0] not in methods:
        return utils.error('Please specify a valid method.')
    
    if (not args[1].startswith('http://')) and (not args[1].startswith('https://')):
       url: str = 'http://' + args[1]
    else:
        url: str = args[1]
    
    params: dict = {}
    json_args: dict = {}
    data: dict = {}

    for i in kwargs:
        if (not i == 'json') and (not i == 'data'):
            params[i] = kwargs[i]
        
        if i == 'json':
            json_args = kwargs[i]
        
        if i == 'data':
            data = kwargs[i]
    try:
        resp: requests.Response = methods[args[0]](url, params=params, data=data, json=json_args)
    except:
        raise errors.InvalidArgument('Please specify a valid URL.')
    
    final: str = utils.error(f'Response {resp.status_code}\n') if (not resp.status_code < 300) and (not resp.status_code > 200) else utils.make_success(f'Response {resp.status_code}\n')

    if 'json' in flags:
        try:
            json_resp: dict = resp.json()
            formatted = utils.format_json(json_resp)

            final += formatted + '\n'
        except:
            final += utils.make_error(f'\nNo JSON Response.')

    if 'text' in flags:
        final += resp.text + '\n'
    
    if 'headers' in flags:
        for i in resp.headers:
            

    return utils.success(final)
    """
