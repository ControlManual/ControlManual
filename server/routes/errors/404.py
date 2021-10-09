from fastapi import Request
from fastapi.responses import JSONResponse

error = 404

def handler(request: Request, exc) -> JSONResponse:
    return JSONResponse(
        {
            "error": "Not Found",
            "status": 404
        }, status_code = 404
    )