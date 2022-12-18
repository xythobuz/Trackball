cmake_minimum_required(VERSION 3.13)

# initialize pico-sdk from submodule
# note: this must happen before project()
include(pico-sdk/pico_sdk_init.cmake)

project(trackball)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

add_executable(trackball)

target_sources(trackball PUBLIC
    src/main.c
    src/log.c
    src/pmw3360.c
    src/usb.c
    src/usb_cdc.c
    src/usb_descriptors.c
    src/usb_hid.c
)

# Make sure TinyUSB can find tusb_config.h
target_include_directories(trackball PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

# pull in common dependencies
target_link_libraries(trackball
    pico_stdlib
    pico_unique_id
    tinyusb_device
    tinyusb_board
    hardware_spi
)

# fix for Errata RP2040-E5 (the fix requires use of GPIO 15)
target_compile_definitions(trackball PUBLIC PICO_RP2040_USB_DEVICE_ENUMERATION_FIX=1)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(trackball)
