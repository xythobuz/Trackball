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
#include "fat_disk.h"
#include "buttons.h"
#include "controls.h"

int main(void) {
    heartbeat_init();
    buttons_init();
    controls_init();

    cnsl_init();
    usb_init();

    if (watchdog_caused_reboot()) {
        debug("reset by watchdog");
    }

    debug("fat_disk_init");
    fat_disk_init();

    debug("pmw_init");
    bool use_pmw = true;
    if (pmw_init() != 0) {
        debug("error initializing PMW3360");
        use_pmw = false;
    }

    // trigger after 500ms
    // (PMW3360 initialization takes ~160ms)
    watchdog_enable(500, 1);

    debug("init done");

    while (1) {
        watchdog_update();

        heartbeat_run();
        buttons_run();
        usb_run();
        cnsl_run();

        if (use_pmw) {
            pmw_run();
        }
    }

    return 0;
}
