#!/bin/bash

cp /usr/local/share/player/{README.txt,screenshot.png} /mnt/USB/

sync

if [ -e /mnt/USB/inhibit.txt ]; then
    exit 1
fi

/usr/local/bin/player /mnt/USB/x.wav /mnt/USB/log

sync
