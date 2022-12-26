/*
 * config.h
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

#endif // __CONFIG_H__
