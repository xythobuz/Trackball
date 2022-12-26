/*
 * main.c
 */

#include "pico/stdlib.h"
#include "hardware/watchdog.h"

#include "config.h"
#include "util.h"
#include "console.h"
#include "log.h"
#include "usb.h"
#include "pmw3360.h"

int main(void) {
    heartbeat_init();

    cnsl_init();
    usb_init();

    if (watchdog_caused_reboot()) {
        debug("reset by watchdog");
    }

    if (pmw_init() != 0) {
        debug("error initializing PMW3360");
    }

    // trigger after 500ms
    // (PMW3360 initialization takes ~160ms)
    watchdog_enable(500, 1);

    debug("init done");

    while (1) {
        watchdog_update();

        heartbeat_run();
        usb_run();
        cnsl_run();
        pmw_run();
    }

    return 0;
}
