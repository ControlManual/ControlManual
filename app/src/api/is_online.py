import requests

def is_online() -> bool:
    """Function for checking if the api is online."""
    try:
        resp = requests.get("https://api.controlmanual.xyz/")

        if resp.status_code == 502:
            raise Exception()

        return True
    except:
        return False