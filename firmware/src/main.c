/*
 * main.c
 */

#include "pico/stdlib.h"

#include "log.h"
#include "usb.h"
#include "pmw3360.h"

int main(void) {
    usb_init();
    pmw_init();
    debug("init done");

    while (1) {
        usb_run();
    }

    return 0;
}
