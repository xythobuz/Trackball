/*
 * Extended from TinyUSB example code.
 *
 * Copyright (c) 2022 - 2023 Thomas Buck (thomas@xythobuz.de)
 *
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
#include "controls.h"
#include "usb_descriptors.h"
#include "usb_hid.h"

static void send_hid_report(uint8_t report_id, uint32_t btn) {
    // skip if hid is not ready yet
    if ( !tud_hid_ready() ) return;

    switch(report_id) {
        case REPORT_ID_KEYBOARD:
        {
            // use to avoid send multiple consecutive zero report for keyboard
            static bool has_keyboard_key = false;

            if ( btn ) {
                uint8_t keycode[6] = { 0 };
                keycode[0] = HID_KEY_A;

                tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, keycode);
                has_keyboard_key = true;
            } else {
                // send empty key report if previously has key pressed
                if (has_keyboard_key) tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, NULL);
                has_keyboard_key = false;
            }
        }
        break;

        case REPORT_ID_MOUSE:
        {
            struct mouse_state mouse = controls_mouse_read();

            uint8_t buttons = 0x00;
            if (mouse.button[MOUSE_LEFT]) {
                buttons |= MOUSE_BUTTON_LEFT;
            }
            if (mouse.button[MOUSE_RIGHT]) {
                buttons |= MOUSE_BUTTON_RIGHT;
            }
            if (mouse.button[MOUSE_MIDDLE]) {
                buttons |= MOUSE_BUTTON_MIDDLE;
            }
            if (mouse.button[MOUSE_BACK]) {
                buttons |= MOUSE_BUTTON_BACKWARD;
            }

            if (mouse.changed) {
                tud_hid_mouse_report(REPORT_ID_MOUSE, buttons,
                        mouse.delta_x * (INVERT_MOUSE_X_AXIS ? -1 : 1),
                        mouse.delta_y * (INVERT_MOUSE_Y_AXIS ? -1 : 1),
                        mouse.scroll_y * (INVERT_SCROLL_Y_AXIS ? -1 : 1),
                        mouse.scroll_x * (INVERT_SCROLL_X_AXIS ? -1 : 1));
            }
        }
        break;

        case REPORT_ID_CONSUMER_CONTROL:
        {
            // use to avoid send multiple consecutive zero report
            //static bool has_consumer_key = false;

            if (btn) {
                // volume down
                //uint16_t volume_down = HID_USAGE_CONSUMER_VOLUME_DECREMENT;
                //tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &volume_down, 2);
                //has_consumer_key = true;
            } else {
                // send empty key report (release key) if previously has key pressed
                //uint16_t empty_key = 0;
                //if (has_consumer_key) tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &empty_key, 2);
                //has_consumer_key = false;
            }
        }
        break;

        case REPORT_ID_GAMEPAD:
        {
            // use to avoid send multiple consecutive zero report for keyboard
            //static bool has_gamepad_key = false;

            //hid_gamepad_report_t report = {
            //    .x   = 0, .y = 0, .z = 0, .rz = 0, .rx = 0, .ry = 0,
            //    .hat = 0, .buttons = 0
            //};

            if (btn) {
                //report.hat = GAMEPAD_HAT_UP;
                //report.buttons = GAMEPAD_BUTTON_A;
                //tud_hid_report(REPORT_ID_GAMEPAD, &report, sizeof(report));
                //has_gamepad_key = true;
            } else {
                //report.hat = GAMEPAD_HAT_CENTERED;
                //report.buttons = 0;
                //if (has_gamepad_key) tud_hid_report(REPORT_ID_GAMEPAD, &report, sizeof(report));
                //has_gamepad_key = false;
            }
        }
        break;

        default:
        break;
    }
}

// Every 10ms, we will sent 1 report for each HID profile (keyboard, mouse etc ..)
// tud_hid_report_complete_cb() is used to send the next report after previous one is complete
void hid_task(void) {
    // Poll every 10ms
    const uint32_t interval_ms = 10;
    static uint32_t start_ms = 0;

    if ( board_millis() - start_ms < interval_ms) return; // not enough time
    start_ms += interval_ms;

    uint32_t const btn = 0;

    // Remote wakeup
    if ( tud_suspended() && btn ) {
        // Wake up host if we are in suspend mode
        // and REMOTE_WAKEUP feature is enabled by host
        tud_remote_wakeup();
    } else {
        // Send the 1st of report chain, the rest will be sent by tud_hid_report_complete_cb()
        //send_hid_report(REPORT_ID_KEYBOARD, btn);
        send_hid_report(REPORT_ID_MOUSE, btn);
    }
}

// Invoked when sent REPORT successfully to host
// Application can use this to send the next report
// Note: For composite reports, report[0] is report ID
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const* report, uint8_t len) {
    (void) instance;
    (void) len;

    uint8_t next_report_id = report[0] + 1;

    if (next_report_id < REPORT_ID_COUNT) {
        send_hid_report(next_report_id, board_button_read());
    }
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id,
        hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen) {
    // TODO not Implemented
    (void) instance;
    (void) report_id;
    (void) report_type;
    (void) buffer;
    (void) reqlen;

    return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id,
        hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize) {
    (void) instance;

    if (report_type == HID_REPORT_TYPE_OUTPUT) {
        // Set keyboard LED e.g Capslock, Numlock etc...
        if (report_id == REPORT_ID_KEYBOARD) {
            // bufsize should be (at least) 1
            if ( bufsize < 1 ) return;

            uint8_t const kbd_leds = buffer[0];

            if (kbd_leds & KEYBOARD_LED_CAPSLOCK) {
                // Capslock On: disable blink, turn led on
                //blink_interval_ms = 0;
                board_led_write(true);
            } else {
                // Caplocks Off: back to normal blink
                board_led_write(false);
                //blink_interval_ms = BLINK_MOUNTED;
            }
        }
    }
}
