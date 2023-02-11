/*
 * Extended from TinyUSB example code.
 *
 * Copyright (c) 2022 - 2023 Thomas Buck (thomas@xythobuz.de)
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "bsp/board.h"
#include "tusb.h"

#include "config.h"
#include "fat_disk.h"
#include "log.h"

static bool ejected = false;
static bool medium_available = false;

bool msc_is_medium_available(void) {
    return medium_available;
}

void msc_set_medium_available(bool state) {
    medium_available = state;
    if (state) {
        ejected = false;
    }
}

// Invoked when received SCSI_CMD_INQUIRY
// Application fill vendor id, product id and revision
// with string up to 8, 16, 4 characters respectively
void tud_msc_inquiry_cb(uint8_t lun, uint8_t vendor_id[8],
        uint8_t product_id[16], uint8_t product_rev[4]) {
    (void) lun;

    const char vid[] = "xythobuz";
    const char pid[] = "Debug Storage";
    const char rev[] = "0.01";

    memcpy(vendor_id  , vid, strlen(vid));
    memcpy(product_id , pid, strlen(pid));
    memcpy(product_rev, rev, strlen(rev));
}

// Invoked when received Test Unit Ready command.
// return true allowing host to read/write this LUN e.g SD card inserted
bool tud_msc_test_unit_ready_cb(uint8_t lun) {
    if (!medium_available) {
        // Additional Sense 3A-00 is NOT_FOUND
        tud_msc_set_sense(lun, SCSI_SENSE_NOT_READY, 0x3a, 0x00);
    }

    return medium_available;
}

// Invoked when received SCSI_CMD_READ_CAPACITY_10 and
// SCSI_CMD_READ_FORMAT_CAPACITY to determine the disk size
// Application update block count and block size
void tud_msc_capacity_cb(uint8_t lun, uint32_t* block_count, uint16_t* block_size) {
    (void) lun;

    if (!medium_available) {
        *block_count = 0;
        *block_size = 0;
    } else {
        *block_count = DISK_BLOCK_COUNT;
        *block_size  = DISK_BLOCK_SIZE;
    }
}

// Invoked when received Start Stop Unit command
// - Start = 0 : stopped power mode, if load_eject = 1 : unload disk storage
// - Start = 1 : active mode, if load_eject = 1 : load disk storage
bool tud_msc_start_stop_cb(uint8_t lun, uint8_t power_condition,
        bool start, bool load_eject) {
    (void) lun;
    (void) power_condition;

    if (start) {
        // load disk storage
        debug("load disk storage %d", load_eject);
    } else {
        // unload disk storage
        debug("unload disk storage %d", load_eject);
        if (load_eject) {
            medium_available = false;
        }
    }

    return true;
}

// Callback invoked when received READ10 command.
// Copy disk's data to buffer (up to bufsize) and return number of copied bytes.
int32_t tud_msc_read10_cb(uint8_t lun, uint32_t lba,
        uint32_t offset, void* buffer, uint32_t bufsize) {
    (void) lun;

    // out of ramdisk
    if (lba >= DISK_BLOCK_COUNT) {
        return -1;
    }

    // TODO better range checking and length calculation?

    uint8_t const* addr = fat_disk_get_sector(lba) + offset;
    memcpy(buffer, addr, bufsize);

    return (int32_t) bufsize;
}

bool tud_msc_is_writable_cb (uint8_t lun) {
    (void) lun;

    return true;
}

// Callback invoked when received WRITE10 command.
// Process data in buffer to disk's storage and return number of written bytes
int32_t tud_msc_write10_cb(uint8_t lun, uint32_t lba,
        uint32_t offset, uint8_t* buffer, uint32_t bufsize) {
    (void) lun;

    // out of ramdisk
    if (lba >= DISK_BLOCK_COUNT) {
        return -1;
    }

    // TODO better range checking and length calculation?

    uint8_t* addr = fat_disk_get_sector(lba) + offset;
    memcpy(addr, buffer, bufsize);

    return (int32_t) bufsize;
}

// Callback invoked when received an SCSI command not in built-in list below
// - READ_CAPACITY10, READ_FORMAT_CAPACITY, INQUIRY, MODE_SENSE6, REQUEST_SENSE
// - READ10 and WRITE10 has their own callbacks and MUST not be handled here
int32_t tud_msc_scsi_cb (uint8_t lun, uint8_t const scsi_cmd[16],
        void* buffer, uint16_t bufsize) {
    void const* response = NULL;
    int32_t resplen = 0;

    // most scsi handled is input
    bool in_xfer = true;

    switch (scsi_cmd[0]) {
    case 0x1E:
        // Prevent/Allow Medium Removal
        if (scsi_cmd[4] & 0x01) {
            // Prevent medium removal
            if (!medium_available) {
                debug("Host wants to lock non-existing medium. Not supported.");
                resplen = -1;
            } else {
                debug("Host wants to lock medium.");
            }
        } else {
            // Allow medium removal
            if (medium_available) {
                debug("Host ejected medium. Unplugging disk.");
                medium_available = false;
            } else {
                debug("host ejected non-existing medium. Not supported.");
                resplen = -1;
            }
        }
        break;

    default:
        // Set Sense = Invalid Command Operation
        tud_msc_set_sense(lun, SCSI_SENSE_ILLEGAL_REQUEST, 0x20, 0x00);

        // negative means error -> tinyusb could stall and/or response with failed status
        resplen = -1;
        break;
    }

    // return resplen must not larger than bufsize
    if (resplen > bufsize) {
        resplen = bufsize;
    }

    if (response && (resplen > 0)) {
        if (in_xfer) {
            memcpy(buffer, response, (size_t) resplen);
        } else {
            // SCSI output
        }
    }

    return (int32_t) resplen;
}
