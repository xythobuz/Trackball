/*
 * controls.h
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

#ifndef __CONTROLS_H__
#define __CONTROLS_H__

enum mouse_buttons {
    MOUSE_LEFT = 0,
    MOUSE_MIDDLE,
    MOUSE_RIGHT,
    MOUSE_BACK,
    MOUSE_FORWARD,
    MOUSE_BUTTONS_COUNT
};

struct mouse_state {
    bool changed;
    bool button[MOUSE_BUTTONS_COUNT];
    int16_t delta_x, delta_y;
    int16_t scroll_x, scroll_y;
    bool scroll_lock;
    uint16_t fake_middle;
    int16_t internal_scroll_x, internal_scroll_y;
};

void controls_init(void);

void controls_mouse_new(int id, bool state);
struct mouse_state controls_mouse_read(void);

#endif // __CONTROLS_H__

