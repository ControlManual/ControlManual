from github import Github
import os
from ..constants import cm_dir

async def download_repo(repo_name: str, ignored: list = ['.gitignore'], repo_path: str = '.'):
    """Function for downloading a GitHub repository to the commands directory."""
    github = Github()
    repo = github.get_repo(repo_name)

    for i in repo.get_contents(repo_path): # type: ignore
        if i.name in ignored:
            continue
        
        name: str = os.path.join(cm_dir, 'commands', i.path)

        if i.type == 'dir':
            os.makedirs(name)
            await download_repo(repo_name, ignored, i.path)
        else:
            with open(name, 'w') as f:
                f.write(i.decoded_content.decode('utf8'))