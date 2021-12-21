import requests


def is_online() -> bool:
    """Function for checking if the api is online."""
    try:
        resp = requests.get("https://api.controlmanual.xyz/")
        if not resp.status_code == 200:
            raise Exception()

        return True
    except:
        return False
