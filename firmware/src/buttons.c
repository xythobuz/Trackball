/*
 * buttons.c
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

#include "config.h"
#include "controls.h"
#include "buttons.h"

#define BUTTONS_COUNT 4
uint gpio_num[BUTTONS_COUNT] = { 21, 22, 26, 27 };

struct button_state {
    uint32_t last_time;
    bool current_state, last_state;
};

struct button_state buttons[BUTTONS_COUNT];

void buttons_init(void) {
    for (int i = 0; i < BUTTONS_COUNT; i++) {
        gpio_init(gpio_num[i]);
        gpio_set_dir(gpio_num[i], GPIO_IN);
        gpio_pull_up(gpio_num[i]);

        buttons[i].last_time = 0;
        buttons[i].current_state = false;
        buttons[i].last_state = false;
    }
}

void buttons_run(void) {
    for (int i = 0; i < BUTTONS_COUNT; i++) {
        bool state = !gpio_get(gpio_num[i]);
        uint32_t now = to_ms_since_boot(get_absolute_time());

        if (state != buttons[i].last_state) {
            buttons[i].last_time = now;
        }

        if ((now - buttons[i].last_time) > DEBOUNCE_DELAY_MS) {
            if (state != buttons[i].current_state) {
                buttons[i].current_state = state;
                controls_mouse_new(i, state);
            }
        }

        buttons[i].last_state = state;
    }
}

