# RP2040 Trackball Firmware

For use with Raspberry Pi Pico boards.

Adapted from the `dev_hid_composite` pico-example [from GitHub](https://github.com/raspberrypi/pico-examples/tree/master/usb/device/dev_hid_composite), as well as the tinyusb-cdc-example [from GitHub](https://github.com/hathach/tinyusb/blob/master/examples/device/cdc_msc/src/main.c).

Supports PMW3360 optical mouse sensors.

## Quick Start

When compiling for the first time, check out the required git submodules.

    git submodule update --init
    cd pico-sdk
    git submodule update --init

Then do this to build.

    mkdir build
    cd build
    cmake ..
    make trackball

And flash the resulting `trackball.uf2` file to your Pico as usual.

For convenience you can use the included `flash.sh`, as long as you flashed the binary manually once before.

    make trackball
    ../flash.sh trackball.uf2

For debugging a serial port will be presented by the firmware.
Open it using eg. `picocom`, or with the included `debug.sh` script.
