#!/bin/bash
set -euo pipefail

SERIAL=/dev/serial/by-id/usb-xythobuz_Trackball_*
DISK=/dev/disk/by-label/RPI-RP2
MOUNT=/mnt/tmp

if [ ! -e $DISK ]
then
    echo Resetting Raspberry Pi Pico
    echo -n -e "\\x18" > $SERIAL
fi

echo -n Waiting for disk to appear
until [ -e $DISK ]
do
    echo -n .
    sleep 1
done
echo

echo Mounting bootloader disk
sudo mount $DISK $MOUNT

echo Copying binary
sudo cp $1 $MOUNT

echo Done
