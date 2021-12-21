import requests


def latest_version() -> str:
    """Function for getting the latest version of Control Manual."""
    return requests.get("https://api.controlmanual.xyz/version/latest").json()[
        "version"
    ]
