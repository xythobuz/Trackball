/*
 * config.h
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

#ifndef __CONFIG_H__
#define __CONFIG_H__

#define PMW_MOTION_PIN 20

#define PMW_PRINT_IDS
#define PMW_IRQ_COUNTERS
//#define PMW_FEATURE_WIRELESS
//#define DISABLE_CDC_DTR_CHECK

#define INVERT_MOUSE_X_AXIS false
#define INVERT_MOUSE_Y_AXIS true
#define DEFAULT_MOUSE_SENSITIVITY PMW_CPI_TO_SENSE(500)
#define DEFAULT_MOUSE_ANGLE -30

#define INVERT_SCROLL_X_AXIS false
#define INVERT_SCROLL_Y_AXIS false
#define SCROLL_REDUCE_SENSITIVITY 20
#define MIN_SCROLL_SUPPRESS_CLICK 10
#define MOUSE_FAKE_MIDDLE_CLICK_TIME 10

#define DEBOUNCE_DELAY_MS 5

#endif // __CONFIG_H__
