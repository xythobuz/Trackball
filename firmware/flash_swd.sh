#!/bin/bash
set -euo pipefail

./openocd/src/openocd -s openocd/tcl -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c "adapter speed 5000" -c "cmsis_dap_vid_pid 0x2e8a 0x000c" -c "program $1 verify reset exit"
