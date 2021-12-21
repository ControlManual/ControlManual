import zipfile
import requests


def download_package(package: str, file: str, target: str) -> bool:
    resp = requests.get(
        "http://localhost:5000/package/get", params={"package": package}
    )

    if not resp.status_code == 200:
        return False

    with open(file, "wb") as f:
        f.write(resp.content)

    with zipfile.ZipFile(file, "r") as zip_ref:
        zip_ref.extractall(target)

    return True
