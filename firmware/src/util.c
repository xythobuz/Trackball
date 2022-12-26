/*
 * util.c
 */

#include <string.h>
#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "hardware/watchdog.h"

#include "config.h"
#include "log.h"
#include "util.h"

#define HEARTBEAT_INTERVAL_MS 500

#ifdef PICO_DEFAULT_LED_PIN
static uint32_t last_heartbeat = 0;
#endif // PICO_DEFAULT_LED_PIN

void heartbeat_init(void) {
#ifdef PICO_DEFAULT_LED_PIN
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    gpio_put(PICO_DEFAULT_LED_PIN, 1);
#endif // PICO_DEFAULT_LED_PIN
}

void heartbeat_run(void) {
#ifdef PICO_DEFAULT_LED_PIN
    uint32_t now = to_ms_since_boot(get_absolute_time());
    if (now >= (last_heartbeat + HEARTBEAT_INTERVAL_MS)) {
        last_heartbeat = now;
        gpio_xor_mask(1 << PICO_DEFAULT_LED_PIN);
    }
#endif // PICO_DEFAULT_LED_PIN
}

bool str_startswith(const char *str, const char *start) {
    size_t l = strlen(start);
    if (l > strlen(str)) {
        return false;
    }
    return (strncmp(str, start, l) == 0);
}

int32_t convert_two_complement(int32_t b) {
    if (b & 0x8000) {
        b = -1 * ((b ^ 0xffff) + 1);
    }
    return b;
}

void reset_to_bootloader(void) {
#ifdef PICO_DEFAULT_LED_PIN
    reset_usb_boot(1 << PICO_DEFAULT_LED_PIN, 0);
#else // ! PICO_DEFAULT_LED_PIN
    reset_usb_boot(0, 0);
#endif // PICO_DEFAULT_LED_PIN
}

void reset_to_main(void) {
    watchdog_enable(1, 1);
    while (1) {
        // wait 1ms until watchdog kills us
        asm volatile("nop");
    }
}

void hexdump(uint8_t *buff, size_t len) {
    for (size_t i = 0; i < len; i += 16) {
        for (size_t j = 0; (j < 16) && ((i + j) < len); j++) {
            print("0x%02X", buff[i + j]);
            if ((j < 15) && ((i + j) < (len - 1))) {
                print(" ");
            }
        }
        println();
    }
}
