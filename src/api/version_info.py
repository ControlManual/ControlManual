import requests
from typing import Union, Dict


def version_info(version: str) -> Union[None, Dict[str, str]]:
    """Function for getting info on a version."""
    resp = requests.get(
        "https://api.controlmanual.xyz/version/info", params={"version": version}
    )

    if not resp.status_code == 200:
        return None
    else:
        json = resp.json()
        return json.get("info")
