/*
 * controls.h
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
    bool scroll_lock, fake_middle;
};

void controls_init(void);

void controls_mouse_new(int id, bool state);
struct mouse_state controls_mouse_read(void);

#endif // __CONTROLS_H__

