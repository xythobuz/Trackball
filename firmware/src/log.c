/*
 * log.c
 */

#include <stdarg.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "usb.h"
#include "log.h"

char log_buff[4096];
size_t head = 0, tail = 0;
bool full = false;

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
    if ((l > 0) && (l <= sizeof(buff))) {
        usb_cdc_write(buff, l);
    }

    if (head > tail) {
        usb_cdc_write(log_buff + tail, head - tail);
    } else {
        usb_cdc_write(log_buff + tail, sizeof(log_buff) - tail);
        usb_cdc_write(log_buff, head);
    }

    l = snprintf(buff, sizeof(buff), "\r\n\r\nlive log:\r\n");
    if ((l > 0) && (l <= sizeof(buff))) {
        usb_cdc_write(buff, l);
    }
}

static int format_debug_log(char *buff, size_t len, const char *format, va_list args) {
    int l = vsnprintf(buff, len, format, args);

    if (l < 0) {
        // encoding error
        l = snprintf(buff, len, "%s: encoding error\r\n", __func__);
    } else if (l >= len) {
        // not enough space for string
        l = snprintf(buff, len, "%s: message too long (%d)\r\n", __func__, l);
    }

    return l;
}

void debug_log(const char* format, ...) {
    static char line_buff[256];

    va_list args;
    va_start(args, format);
    int l = format_debug_log(line_buff, sizeof(line_buff), format, args);
    va_end(args);

    if ((l > 0) && (l <= sizeof(line_buff))) {
        usb_cdc_write(line_buff, l);
        add_to_log(line_buff, l);
    }
}
