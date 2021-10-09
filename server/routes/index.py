from fastapi import APIRouter

router = APIRouter()
prefix = ''

@router.get('/')
def index() -> dict:
    return {
        'message': "Welcome to the Control Manual API!",
        'status': 200
    }