from fastapi import APIRouter, Response
from fastapi.responses import FileResponse
from utils import directory
import shutil
import json
import os

#shutil.make_archive(output_filename, 'zip', dir_name)

router = APIRouter()
prefix = '/package'

@router.get('/')
def index() -> dict:
    return {
        'status': 200
    }

@router.get('/get')
def download(package: str, response: Response) -> dict:
    with open(os.path.join(directory, 'packages', 'packages.json')) as f:
        load = json.load(f)
    
    package_dir = load.get(package)

    if not package_dir:
        response.status_code = 404
        return {
            'message': 'Package not found',
            'status': 404
        }
    path: str = os.path.join(directory, 'packages', 'tmp', f'{package}')
    shutil.make_archive(path, 'zip', package_dir)

    return FileResponse(path + '.zip', status_code = 200)