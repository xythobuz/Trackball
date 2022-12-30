/*
 * controls.c
 */

#include "pico/stdlib.h"

#include "config.h"
#include "controls.h"

static bool button_state = false;

void controls_new(int id, bool state) {
    (void)id;
    button_state = state;
}

bool controls_button_read(void) {
    return button_state;
}

