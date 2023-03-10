/*
 * log.c
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

#include <stdarg.h>
#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/watchdog.h"
#include "ff.h"

#include "config.h"
#include "usb.h"
#include "usb_cdc.h"
#include "log.h"

static char log_buff[4096];
static size_t head = 0, tail = 0;
static bool full = false;
static bool got_input = false;

static void add_to_log(const char *buff, int len) {
    for (int i = 0; i < len; i++) {
        log_buff[head] = buff[i];

        if (full && (++tail == sizeof(log_buff))) {
            tail = 0;
        }

        if (++(head) == sizeof(log_buff)) {
            head = 0;
        }

        full = (head == tail);
    }
}

void log_dump_to_usb(void) {
    if (head == tail) {
        return;
    }

    char buff[32];
    int l = snprintf(buff, sizeof(buff), "\r\n\r\nbuffered log output:\r\n");
    if ((l > 0) && (l <= (int)sizeof(buff))) {
        usb_cdc_write(buff, l);
    }

    if (head > tail) {
        usb_cdc_write(log_buff + tail, head - tail);
    } else {
        usb_cdc_write(log_buff + tail, sizeof(log_buff) - tail);
        usb_cdc_write(log_buff, head);
    }

    l = snprintf(buff, sizeof(buff), "\r\n\r\nlive log:\r\n");
    if ((l > 0) && (l <= (int)sizeof(buff))) {
        usb_cdc_write(buff, l);
    }
}

void log_dump_to_disk(void) {
    FIL file;
    FRESULT res = f_open(&file, "log.txt", FA_CREATE_ALWAYS | FA_WRITE);
    if (res != FR_OK) {
        debug("error: f_open returned %d", res);
        return;
    }

    UINT bw;

    if (head > tail) {
        res = f_write(&file, log_buff + tail, head - tail, &bw);
        if ((res != FR_OK) || (bw != head - tail)) {
            debug("error: f_write (A) returned %d", res);
        }
    } else if (head < tail) {
        res = f_write(&file, log_buff + tail, sizeof(log_buff) - tail, &bw);
        if ((res != FR_OK) || (bw != sizeof(log_buff) - tail)) {
            debug("error: f_write (B) returned %d", res);
        } else {
            res = f_write(&file, log_buff, head, &bw);
            if ((res != FR_OK) || (bw != head)) {
                debug("error: f_write (C) returned %d", res);
            }
        }
    }

    res = f_close(&file);
    if (res != FR_OK) {
        debug("error: f_close returned %d", res);
    }
}

static int format_debug_log(char *buff, size_t len, const char *format, va_list args) {
    int l = vsnprintf(buff, len, format, args);

    if (l < 0) {
        // encoding error
        l = snprintf(buff, len, "%s: encoding error\r\n", __func__);
    } else if (l >= (ssize_t)len) {
        // not enough space for string
        l = snprintf(buff, len, "%s: message too long (%d)\r\n", __func__, l);
    }

    return l;
}

void debug_log(bool log, const char* format, ...) {
    static char line_buff[512];

    va_list args;
    va_start(args, format);
    int l = format_debug_log(line_buff, sizeof(line_buff), format, args);
    va_end(args);

    if ((l > 0) && (l <= (int)sizeof(line_buff))) {
        usb_cdc_write(line_buff, l);

        if (log) {
            add_to_log(line_buff, l);
        }
    }
}

void debug_handle_input(char *buff, uint32_t len) {
    (void)buff;

    if (len > 0) {
        got_input = true;
    }
}

void debug_wait_input(const char *format, ...) {
    static char line_buff[512];

    va_list args;
    va_start(args, format);
    int l = format_debug_log(line_buff, sizeof(line_buff), format, args);
    va_end(args);

    if ((l > 0) && (l <= (int)sizeof(line_buff))) {
        usb_cdc_write(line_buff, l);
    }

    got_input = false;
    usb_cdc_set_reroute(true);

    while (!got_input) {
        watchdog_update();
        usb_run();
    }

    usb_cdc_set_reroute(false);
}
