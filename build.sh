#!/usr/bin/sh
gcc -shared -o libcontrolmanual.so -fPIC src/**/*.c -I include -Wall -lm
if [[ $? -ne 0 ]]; then
    exit 1
fi

sudo cp libcontrolmanual.so /usr/lib/libcontrolmanual.so
sudo cp include/controlmanual /usr/include/controlmanual -r