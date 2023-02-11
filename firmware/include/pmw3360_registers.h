/*
 * pmw3360_registers.h
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

#ifndef __PMW3360_REGISTERS_H__
#define __PMW3360_REGISTERS_H__

#define REG_PRODUCT_ID                 0x00
#define REG_REVISION_ID                0x01
#define REG_MOTION                     0x02
#define REG_DELTA_X_L                  0x03
#define REG_DELTA_X_H                  0x04
#define REG_DELTA_Y_L                  0x05
#define REG_DELTA_Y_H                  0x06
#define REG_SQUAL                      0x07
#define REG_RAW_DATA_SUM               0x08
#define REG_MAXIMUM_RAW_DATA           0x09
#define REG_MINIMUM_RAW_DATA           0x0A
#define REG_SHUTTER_LOWER              0x0B
#define REG_SHUTTER_UPPER              0x0C
#define REG_CONTROL                    0x0D
#define REG_CONFIG1                    0x0F
#define REG_CONFIG2                    0x10
#define REG_ANGLE_TUNE                 0x11
#define REG_FRAME_CAPTURE              0x12
#define REG_SROM_ENABLE                0x13
#define REG_RUN_DOWNSHIFT              0x14
#define REG_REST1_RATE_LOWER           0x15
#define REG_REST1_RATE_UPPER           0x16
#define REG_REST1_DOWNSHIFT            0x17
#define REG_REST2_RATE_LOWER           0x18
#define REG_REST2_RATE_UPPER           0x19
#define REG_REST2_DOWNSHIFT            0x1A
#define REG_REST3_RATE_LOWER           0x1B
#define REG_REST3_RATE_UPPER           0x1C
#define REG_OBSERVATION                0x24
#define REG_DATA_OUT_LOWER             0x25
#define REG_DATA_OUT_UPPER             0x26
#define REG_RAW_DATA_DUMP              0x29
#define REG_SROM_ID                    0x2A
#define REG_MIN_SQ_RUN                 0x2B
#define REG_RAW_DATA_THRESHOLD         0x2C
#define REG_CONFIG5                    0x2F
#define REG_POWER_UP_RESET             0x3A
#define REG_SHUTDOWN                   0x3B
#define REG_INVERSE_PRODUCT_ID         0x3F
#define REG_LIFTCUTOFF_TUNE3           0x41
#define REG_ANGLE_SNAP                 0x42
#define REG_LIFTCUTOFF_TUNE1           0x4A
#define REG_MOTION_BURST               0x50
#define REG_LIFTCUTOFF_TUNE_TIMEOUT    0x58
#define REG_LIFTCUTOFF_TUNE_MIN_LENGTH 0x5A
#define REG_SROM_LOAD_BURST            0x62
#define REG_LIFT_CONFIG                0x63
#define REG_RAW_DATA_BURST             0x64
#define REG_LIFTCUTOFF_TUNE2           0x65

#define WRITE_BIT 0x80

#define REG_MOTION_MOT 7
#define REG_MOTION_LIFT 3
#define REG_MOTION_OP_1 1
#define REG_MOTION_OP_2 2

struct pmw_motion_report {
    uint8_t motion;
    uint8_t observation;
    uint8_t delta_x_l;
    uint8_t delta_x_h;
    uint8_t delta_y_l;
    uint8_t delta_y_h;
    uint8_t squal;
    uint8_t raw_data_sum;
    uint8_t maximum_raw_data;
    uint8_t minimum_raw_data;
    uint8_t shutter_upper;
    uint8_t shutter_lower;
} __attribute__((packed));

#endif // __PMW3360_REGISTERS_H__
