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

#define HEALTH_CHECK_INTERVAL_MS 500
static uint32_t last_health_check = 0;

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

        uint32_t now = to_ms_since_boot(get_absolute_time());
        if (now >= (last_health_check + HEALTH_CHECK_INTERVAL_MS)) {
            last_health_check = now;
            if (!pmw_is_alive()) {
                debug("PMW3360 is dead. resetting!");
                while (1) { }
            }
        }
    }

    return 0;
}
