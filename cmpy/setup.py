from setuptools import Extension, setup

if __name__ == "__main__":
    setup(
        ext_modules=[
            Extension(
                "controlmanual",
                ["./mod.c"],
                libraries=['controlmanual']
            )
        ],
        license="MIT",
        package_data={"controlmanual": ["py.typed", "controlmanual.pyi"]}
    )
