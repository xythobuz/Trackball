cmake_minimum_required(VERSION 3.13)

# initialize pico-sdk from submodule
# note: this must happen before project()
include(pico-sdk/pico_sdk_init.cmake)

project(trackball)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# copy FatFS source files to build dir, so we can use our own ffconf.h
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/fatfs/source/ff.c
    ${CMAKE_CURRENT_BINARY_DIR}/fatfs/ff.c
    COPYONLY
)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/fatfs/source/ff.h
    ${CMAKE_CURRENT_BINARY_DIR}/fatfs/ff.h
    COPYONLY
)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/fatfs/source/diskio.h
    ${CMAKE_CURRENT_BINARY_DIR}/fatfs/diskio.h
    COPYONLY
)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/fatfs/source/ffunicode.c
    ${CMAKE_CURRENT_BINARY_DIR}/fatfs/ffunicode.c
    COPYONLY
)

add_executable(trackball)

target_sources(trackball PUBLIC
    src/main.c
    src/console.c
    src/log.c
    src/util.c
    src/pmw3360.c
    src/usb.c
    src/usb_cdc.c
    src/usb_descriptors.c
    src/usb_hid.c
    src/usb_msc.c
    src/fat_disk.c
    src/debug.c
    src/buttons.c
    src/controls.c
    ${CMAKE_CURRENT_BINARY_DIR}/fatfs/ff.c
    ${CMAKE_CURRENT_BINARY_DIR}/fatfs/ffunicode.c
)

# Make sure TinyUSB can find tusb_config.h
target_include_directories(trackball PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

# Make sure FatFS can find ffconf.h
target_include_directories(trackball PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/fatfs)

target_compile_options(trackball PUBLIC
    -Wall
    -Wextra
    -Werror
)

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
