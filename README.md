# Control Manual

## Alternative command interpreter

### What is this?

This is an alternative to traditional command interpreters. I'll put more here as this project progresses, but essentially heres what it's supposed to look like:

```
# Bash
if [[ $something -eq 0 ]]; then
    echo "Something happened"
else
    echo "Something else happened"
fi

ls | grep myfile.txt
```

```
# Control Manual
if something == 0 {
    echo "Something happened"
} else {
    echo "Something else happened"
}

match ls() "myfile.txt"
```

### Installation

Since this is still a heavy WIP, there isn't really any installation process. However, you can clone the repository and build, like so:

```bash
$ git clone https://github.com/ZeroIntensity/controlmanual
$ cd controlmanual
$ gcc yourfile.c -I include src/core/*.c src/engine/*.c -o out -lm
```
