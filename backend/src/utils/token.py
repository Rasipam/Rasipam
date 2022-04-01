from functools import wraps
from uuid import uuid4
from fastapi import Request
from fastapi.responses import JSONResponse
from time import time


tokens = {}
TOKEN_EXPIRATION_TIME = 24 * 60 * 60


def gen_token():
    new_token = str(uuid4())
    while new_token in tokens:
        new_token = str(uuid4())
    tokens[new_token] = time()
    return new_token


def get_token_from_request(request: Request):
    return request.headers.get('token')


def is_valid_token(token):
    if token not in tokens:
        return False
    gen_time = tokens[token]
    return time() - gen_time < TOKEN_EXPIRATION_TIME


def auth_required(func):
    @wraps(func)
    async def wrapper(request, *args, **kwargs):
        try:
            token = get_token_from_request(request)
            assert is_valid_token(token), 'Unauthorized token.'
            return await func(request, *args, **kwargs)
        except AssertionError:
            return JSONResponse(
                    status_code=401,
                    content={"detail": "Unauthorized"}
            )

    return wrapper
