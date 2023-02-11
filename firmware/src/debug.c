/*
 * debug.c
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

#include <string.h>
#include "pico/stdlib.h"
#include "ff.h"

#include "config.h"
#include "log.h"
#include "pmw3360.h"
#include "debug.h"

static FATFS fs;
static bool mounted = false;

int debug_msc_mount(void) {
    if (mounted) {
        debug("already mounted");
        return 0;
    }

    FRESULT res = f_mount(&fs, "", 0);
    if (res != FR_OK) {
        debug("error: f_mount returned %d", res);
        mounted = false;
        return -1;
    }

    mounted = true;
    return 0;
}

int debug_msc_unmount(void) {
    if (!mounted) {
        debug("already unmounted");
        return 0;
    }

    FRESULT res = f_mount(0, "", 0);
    if (res != FR_OK) {
        debug("error: f_mount returned %d", res);
        return -1;
    }

    mounted = false;
    return 0;
}

static void debug_msc_pmw_stats(void) {
    FIL file;
    FRESULT res = f_open(&file, "pmw_stats.txt", FA_CREATE_ALWAYS | FA_WRITE);
    if (res != FR_OK) {
        debug("error: f_open returned %d", res);
        return;
    }

    char status_buff[1024];
    pmw_print_status(status_buff, sizeof(status_buff));
    size_t len = strlen(status_buff);

    UINT bw;
    res = f_write(&file, status_buff, len, &bw);
    if ((res != FR_OK) || (bw != len)) {
        debug("error: f_write returned %d", res);
    }

    res = f_close(&file);
    if (res != FR_OK) {
        debug("error: f_close returned %d", res);
    }
}

void debug_msc_stats(void) {
    debug_msc_pmw_stats();
    log_dump_to_disk();
}

static void debug_msc_pmw3360_frame(void) {
    FIL file;
    FRESULT res = f_open(&file, "pmw_frame.bin", FA_CREATE_ALWAYS | FA_WRITE);
    if (res != FR_OK) {
        debug("error: f_open returned %d", res);
        return;
    }

    uint8_t frame[PMW_FRAME_CAPTURE_LEN];
    ssize_t r = pmw_frame_capture(frame, PMW_FRAME_CAPTURE_LEN);
    if (r != PMW_FRAME_CAPTURE_LEN) {
        debug("error: pmw_frame_capture %d != %d", r, PMW_FRAME_CAPTURE_LEN);
    } else {
        UINT bw;
        res = f_write(&file, frame, r, &bw);
        if ((res != FR_OK) || ((ssize_t)bw != r)) {
            debug("error: f_write returned %d", res);
        }
    }

    res = f_close(&file);
    if (res != FR_OK) {
        debug("error: f_close returned %d", res);
    }
}

void debug_msc_pmw3360(void) {
    pmw_dump_data(false);
    debug_msc_pmw3360_frame();
}
