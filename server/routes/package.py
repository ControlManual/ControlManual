from fastapi import APIRouter, Response, UploadFile, File
from fastapi.responses import FileResponse
from utils import directory
import os

router = APIRouter()
prefix = '/package'

@router.get('/')
def index() -> dict:
    return {
        'status': 200
    }

@router.get('/get')
def download(package: str, response: Response) -> dict:
    path: str = os.path.join(directory, 'packages', f'{package}.zip')
    if not os.path.exists(path):
        response.status_code = 404
        return {
            'error': 'Package not found',
            'status': 404
        }

    return FileResponse(path, status_code = 200)

@router.post('/submit')
async def submit(file: UploadFile = File(...)) -> dict:
    path = os.path.join(
        directory, 
        'packages', 
        file.filename
    )

    open(path, 'w').close()


    with open(path, 'wb') as f:
        f.write(await file.read())
