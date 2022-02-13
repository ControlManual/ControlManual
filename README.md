# Control Manual

![https://github.com/ControlManual/ControlManual](https://github.com/ControlManual/ControlManual/actions/workflows/tests.yml/badge.svg)
![https://pypi.org/project/ControlManual/](https://img.shields.io/pypi/dm/ControlManual)
![https://github.com/ZeroIntensity/ControlManual/blob/main/LICENSE](https://img.shields.io/github/license/ZeroIntensity/ControlManual)
![https://github.com/ZeroIntensity/ControlManual](https://img.shields.io/github/last-commit/ZeroIntensity/ControlManual)

### Running

This has currently only been tested on Linux (Ubuntu to be exact). Some features may not work property on other operating systems as of now.

This may also not work correctly on some terminals, due to the complexity of the TUI.

**Note:** It seems that there are some performance issues on first launch. If you are experience these issues, simply close and reopen the app.

#### Prequisites

- Python 3.8+
- Terminal that supports ANSI/ASCII codes
- Linux or macOS
- Recommended to run in a [truecolor](https://github.com/termstandard/colors) supporting terminal.

#### From Source

```
$ git clone https://github.com/ZeroIntensity/ControlManual && cd ControlManual
$ python3 -m pip install -r requirements.txt
$ python3 setup.py install --user
$ python3 -m controlmanual
```

#### From PyPI

```
$ pip3 install ControlManual
$ python3 -m ControlManual
```
