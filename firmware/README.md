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
    make -j4 trackball

And flash the resulting `trackball.uf2` file to your Pico as usual.

For convenience you can use the included `flash.sh`, as long as you flashed the binary manually once before.

    make -j4 trackball
    ../flash.sh trackball.uf2

This will use the mass storage bootloader to upload a new uf2 image.

For old-school debugging a serial port will be presented by the firmware.
Open it using eg. `picocom`, or with the included `debug.sh` script.

For dependencies to compile, on Arch install these.

    sudo pacman -S arm-none-eabi-gcc arm-none-eabi-newlib picocom cmake

## Proper Debugging

You can also use the SWD interface for proper hardware debugging.

This follows the instructions from the [RP2040 Getting Started document](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf) from chapter 5 and 6.

For ease of reading the disassembly, create a debug build.

    mkdir build_debug
    cd build_debug
    cmake -DCMAKE_BUILD_TYPE=Debug ..
    make -j4 trackball

You need a hardware SWD probe.
This can be made from another Pico, see Appendix A in the document linked above.
For this you need to compile the `picoprobe` firmware, like this.

    git clone https://github.com/raspberrypi/picoprobe.git
    cd picoprobe
    git submodule update --init
    mkdir build
    cd build
    PICO_SDK_PATH=../../../pico-sdk cmake ..
    make -j4

And flash the resulting `picoprobe.uf2` to your probe.
Connect `GP2` of the probe to `SWCLK` of the target and `GP3` of the probe to `SWDIO` of the target.
Of course you also need to connect GND between both.

You need some dependencies, mainly `gdb-multiarch` and the RP2040 fork of `OpenOCD`.

    sudo apt install gdb-multiarch   # Debian / Ubuntu
    sudo pacman -S arm-none-eabi-gdb # Arch Linux

    cd ../.. # back to build_debug directory from before

    git clone https://github.com/raspberrypi/openocd.git --branch rp2040 --recursive --depth=1
    cd openocd

    # install udev rules
    sudo cp contrib/60-openocd.rules /etc/udev/rules.d
    sudo udevadm control --reload-rules && sudo udevadm trigger

    ./bootstrap
    ./configure --enable-ftdi --enable-sysfsgpio --enable-bcm2835gpio
    make -j4

Now we can flash a firmware image via OpenOCD.

    ./openocd/src/openocd -s openocd/tcl -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c "adapter speed 5000" -c "cmsis_dap_vid_pid 0x2e8a 0x000c" -c "program trackball.elf verify reset exit"

And also start a GDB debugging session.

    ./openocd/src/openocd -s openocd/tcl -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c "adapter speed 5000" -c "cmsis_dap_vid_pid 0x2e8a 0x000c"
    arm-none-eabi-gdb trackball.elf
    target extended-remote localhost:3333

These commands have also been put in the `flash_swd.sh` and `debug_swd.sh` scripts, respectively.
Call them from the `build_debug` folder where you checked out and built OpenOCD.
