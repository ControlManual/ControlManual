from fastapi import FastAPI
import uvicorn
import json
from utils import load_routes, directory
import os


app = FastAPI()
with open(os.path.join(directory, "config.json")) as f:
    config: dict = json.load(f)

load_routes(app)
app_conf: dict = config["app"]

if __name__ == "__main__":
    uvicorn.run(app, **app_conf)
