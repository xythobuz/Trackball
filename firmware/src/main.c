/*
 * main.c
 */

#include "pico/stdlib.h"

#include "util.h"
#include "console.h"
#include "log.h"
#include "usb.h"
#include "pmw3360.h"

int main(void) {
    heartbeat_init();
    cnsl_init();
    usb_init();
    pmw_init();
    debug("init done");

    while (1) {
        heartbeat_run();
        usb_run();
    }

    return 0;
}
