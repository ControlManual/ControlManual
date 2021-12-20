from fastapi import Request
from fastapi.responses import JSONResponse
from fastapi.exceptions import RequestValidationError

error = RequestValidationError


def handler(request: Request, exc) -> JSONResponse:
    return JSONResponse({"error": "Missing Arguments", "status": 400}, status_code=400)
