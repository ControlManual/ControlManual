#!/usr/bin/sh
sudo cp -r ./include /usr/
gcc -shared -o libcontrolmanual.so -fPIC src/**/*.c -Wall -lm -D CM_DEBUG -D CM_COMPILING -I /usr/include
if [[ $? -ne 0 ]]; then
    exit 1
fi

sudo cp libcontrolmanual.so /usr/lib/libcontrolmanual.so
sudo cp include/controlmanual /usr/include/controlmanual -r