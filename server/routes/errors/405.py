from fastapi import Request
from fastapi.responses import JSONResponse

error = 500


def handler(request: Request, exc) -> JSONResponse:
    return JSONResponse(
        {"error": "Method not allowed.", "status": 405}, status_code=405
    )
