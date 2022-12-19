/* 
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include "bsp/board.h"
#include "tusb.h"

#include "config.h"
#include "console.h"
#include "log.h"
#include "util.h"
#include "usb_descriptors.h"
#include "usb_cdc.h"

void usb_cdc_write(const char *buf, uint32_t count) {
#ifndef DISABLE_CDC_DTR_CHECK
    if (!tud_cdc_connected()) {
        return;
    }
#endif // DISABLE_CDC_DTR_CHECK

    // implemented similar to Pico SDK stdio usb
    uint32_t len = 0;
    while (len < count) {
        uint32_t n = count - len;
        uint32_t available = tud_cdc_write_available();

        // only write as much as possible
        if (n > available) {
            n = available;
        }

        len += tud_cdc_write(buf + len, n);

        // run tud_task to actually move stuff from FIFO
        tud_task();
        tud_cdc_write_flush();
    }
}

void cdc_task(void) {
    const uint32_t cdc_buf_len = 64;

    if (tud_cdc_available()) {
        char buf[cdc_buf_len + 1];
        uint32_t count = tud_cdc_read(buf, cdc_buf_len);

        if ((count >= 1) && (buf[0] == 0x18)) {
            // ASCII 0x18 = CAN (cancel)
            debug("switching to bootloader");
            reset_to_bootloader();
        } else {
            // echo back
            usb_cdc_write(buf, count);

            cnsl_handle_input(buf, count);
        }
    }
}

// invoked when cdc when line state changed e.g connected/disconnected
void tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts) {
    (void) itf;
    (void) rts;

    static bool last_dtr = false;

    if (dtr && !last_dtr) {
        // clear left-over console input
        cnsl_init();

        // show past history
        log_dump_to_usb();

        debug("terminal connected");
    } else if (!dtr && last_dtr) {
        debug("terminal disconnected");
    }

    last_dtr = dtr;
}

// invoked when CDC interface received data from host
void tud_cdc_rx_cb(uint8_t itf) {
    (void) itf;
    cdc_task();
}
