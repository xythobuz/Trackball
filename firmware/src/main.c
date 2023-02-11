/*
 * main.c
 *
 * Copyright (c) 2022 - 2023 Thomas Buck (thomas@xythobuz.de)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * See <http://www.gnu.org/licenses/>.
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
