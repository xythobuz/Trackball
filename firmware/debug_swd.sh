#!/bin/bash
set -euo pipefail

echo Starting OpenOCD in background
./openocd/src/openocd -s openocd/tcl -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c "adapter speed 5000" -c "cmsis_dap_vid_pid 0x2e8a 0x000c" &
OPENOCD_PID=$!

# give OpenOCD some time to output stuff
sleep 1

echo -n Waiting for debugger to appear
while ! netstat -tna | grep 'LISTEN\>' | grep -q ':3333\>'; do
    echo -n .
    sleep 1
done

echo Starting GDB
arm-none-eabi-gdb -ex "target extended-remote localhost:3333" $1

echo Killing OpenOCD instance in background
kill $OPENOCD_PID
