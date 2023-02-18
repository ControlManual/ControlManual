from setuptools import Extension, setup

if __name__ == "__main__":
    setup(
        name="controlmanual",
        version="1.0.0",
        license="MIT",
        project_urls={
            "Source": "https://github.com/ZeroIntensity/controlmanual",
        },
        ext_modules=[Extension("controlmanual", ["./mod.c"])],
    )