from fastapi import APIRouter

router = APIRouter()
prefix = ''

@router.get('/')
def index() -> None:
    return {'message': "Hello World!"}