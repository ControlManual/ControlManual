from fastapi import APIRouter, Response

router = APIRouter()
prefix = '/version'

VERSION_INFO: dict = {
    'alpha 1.0.0': {
        'released': False,
        'stable': True,
        'latest': False
    }
} # hardcoded for now, will be changed and connected to a database later

@router.get('/')
def index() -> dict:
    return {
        'status': 200
    }

@router.get('/latest')
def index() -> dict:
    return {
        'version': "Alpha 1.0.1", # again, will be changed and connected to a database later
        'status': 200
    }

@router.get('/info')
def index(version: str, response: Response) -> dict:
    if not version.lower() in VERSION_INFO:
        response.status_code = 404
        return {
            'error': 'Version not found',
            'status': 404
        }
    else:
        return {
            'info': VERSION_INFO[version.lower()],
            'status': 200
        }



