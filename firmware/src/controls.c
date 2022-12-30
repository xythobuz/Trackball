/*
 * controls.c
 */

#include <stdlib.h>

#include "pico/stdlib.h"

#include "config.h"
#include "log.h"
#include "pmw3360.h"
#include "controls.h"

static struct mouse_state mouse, last_mouse;
static uint64_t scroll_sum = 0;

void controls_init(void) {
    for (int i = 0; i < MOUSE_BUTTONS_COUNT; i++) {
        mouse.button[i] = false;
    }
    mouse.changed = false;
    mouse.delta_x = 0;
    mouse.delta_y = 0;
    mouse.scroll_x = 0;
    mouse.scroll_y = 0;
    mouse.scroll_lock = false;
    mouse.fake_middle = false;

    last_mouse = mouse;
}

void controls_mouse_new(int id, bool state) {
    //debug("button %d %s", id, state ? "pressed" : "released");

    switch (id) {
    case 0:
        mouse.button[MOUSE_BACK] = state;
        break;

    case 1:
        mouse.scroll_lock = state;
        break;

    case 2:
        mouse.button[MOUSE_LEFT] = state;
        break;

    case 3:
        mouse.button[MOUSE_RIGHT] = state;
        break;
    }
}

static bool mouse_state_changed(struct mouse_state a, struct mouse_state b) {
    for (int i = 0; i < MOUSE_BUTTONS_COUNT; i++) {
        if (a.button[i] != b.button[i]) {
            return true;
        }
    }

    if ((a.delta_x != b.delta_x)
            || (a.delta_y != b.delta_y)
            || (a.scroll_x != b.scroll_x)
            || (a.scroll_y != b.scroll_y)) {
        return true;
    }

    return false;
}

struct mouse_state controls_mouse_read(void) {
    struct pmw_motion motion = pmw_get();

    if (motion.motion) {
        mouse.delta_x = motion.delta_x;
        mouse.delta_y = motion.delta_y;
    }

    mouse.scroll_x = 0;
    mouse.scroll_y = 0;

    if (mouse.scroll_lock) {
        scroll_sum += abs(mouse.delta_x);
        scroll_sum += abs(mouse.delta_y);

        mouse.internal_scroll_x += mouse.delta_x;
        mouse.internal_scroll_y += mouse.delta_y;

        mouse.delta_x = 0;
        mouse.delta_y = 0;

        while (mouse.internal_scroll_x > SCROLL_REDUCE_SENSITIVITY) {
            mouse.scroll_x += 1;
            mouse.internal_scroll_x -= SCROLL_REDUCE_SENSITIVITY;
        }
        while (mouse.internal_scroll_x < -SCROLL_REDUCE_SENSITIVITY) {
            mouse.scroll_x -= 1;
            mouse.internal_scroll_x += SCROLL_REDUCE_SENSITIVITY;
        }

        while (mouse.internal_scroll_y > SCROLL_REDUCE_SENSITIVITY) {
            mouse.scroll_y += 1;
            mouse.internal_scroll_y -= SCROLL_REDUCE_SENSITIVITY;
        }
        while (mouse.internal_scroll_y < -SCROLL_REDUCE_SENSITIVITY) {
            mouse.scroll_y -= 1;
            mouse.internal_scroll_y += SCROLL_REDUCE_SENSITIVITY;
        }
    } else {
        mouse.internal_scroll_x = 0;
        mouse.internal_scroll_y = 0;
    }

    if (mouse.fake_middle) {
        mouse.button[MOUSE_MIDDLE] = false;
        mouse.fake_middle = false;
    }

    if (!mouse.scroll_lock && last_mouse.scroll_lock) {
        // middle mouse button was held and has now been released
        if (scroll_sum < MIN_SCROLL_SUPPRESS_CLICK) {
            // fake middle mouse click, user was not scrolling
            mouse.button[MOUSE_MIDDLE] = true;
            mouse.fake_middle = true;
        }
    }

    if (mouse_state_changed(mouse, last_mouse)
            || (mouse.delta_x != 0)
            || (mouse.delta_y != 0)
            || (mouse.scroll_x != 0)
            || (mouse.scroll_y != 0)) {
        mouse.changed = true;
    }

    last_mouse = mouse;
    return mouse;
}

