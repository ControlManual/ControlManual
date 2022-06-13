from setuptools import setup
from setuptools_rust import Binding, RustExtension
from glob import glob

with open("./README.md") as f:
    long_desc: str = f.read()

if __name__ == "__main__":
    setup(
        name="controlmanual",
        version="1.0.0",
        author="ZeroIntensity",
        author_email="<zintensitydev@gmail.com>",
        description="Advanced command interpreter.",
        long_description_content_type="text/markdown",
        long_description=long_desc,
        packages=["controlmanual"],
        keywords=["python", "commands", "command"],
        install_requires=["typing_extensions"],
        classifiers=[
            "Programming Language :: Python :: 3.6",
            "Programming Language :: Python :: 3.7",
            "Programming Language :: Python :: 3.8",
            "Programming Language :: Python :: 3.9",
            "Programming Language :: Python :: 3.10",
            "Programming Language :: Python :: 3.11",
        ],
        license="MIT",
        project_urls={
            "Source": "https://github.com/ZeroIntensity/ControlManual",
        },
        package_dir={"": "src"},
        rust_extensions=[
            RustExtension(
                "_controlmanual",
                path="./src/native/Cargo.toml",
                binding=Binding.PyO3,
                debug=False,
            )
        ],
        zip_safe=False,
    )
