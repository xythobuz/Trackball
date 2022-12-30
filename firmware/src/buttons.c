/*
 * buttons.c
 */

#include "pico/stdlib.h"

#include "config.h"
#include "controls.h"
#include "buttons.h"

#define BUTTONS_COUNT 4
uint gpio_num[BUTTONS_COUNT] = { 21, 22, 26, 27 };

struct button_state {
    uint32_t last_time;
    bool last_state;
};

struct button_state buttons[BUTTONS_COUNT];

void buttons_init(void) {
    for (int i = 0; i < BUTTONS_COUNT; i++) {
        gpio_init(gpio_num[i]);
        gpio_pull_up(gpio_num[i]);
    }
}

void buttons_run(void) {
    for (int i = 0; i < BUTTONS_COUNT; i++) {
        bool state = gpio_get(gpio_num[i]);
        uint32_t now = to_ms_since_boot(get_absolute_time());

        if (state != buttons[i].last_state) {
            buttons[i].last_time = now;
        }

        if ((now - buttons[i].last_time) > DEBOUNCE_DELAY_MS) {
            if (state != buttons[i].last_state) {
                buttons[i].last_state = state;
                controls_new(i, state);
            }
        }
    }
}

