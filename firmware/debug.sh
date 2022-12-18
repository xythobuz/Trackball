#!/bin/bash
set -euo pipefail

SERIAL=/dev/serial/by-id/usb-xythobuz_Trackball_*

echo -n Waiting for serial port to appear
until [ -e $SERIAL ]
do
    echo -n .
    sleep 1
done
echo

echo Opening picocom terminal
echo "[C-a] [C-x] to exit"
echo

picocom -q --omap crcrlf $SERIAL
