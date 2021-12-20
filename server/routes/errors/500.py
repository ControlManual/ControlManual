from fastapi import Request
from fastapi.responses import JSONResponse

error = 500


def handler(request: Request, exc) -> JSONResponse:
    return JSONResponse(
        {"error": "Internal Server Error", "status": 500}, status_code=500
    )
