import requests

def is_online() -> bool:
    try:
        resp = requests.get("https://api.controlmanual.xyz/")

        if resp.status_code == 502:
            raise Exception()

        return True
    except:
        return False