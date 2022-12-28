/* 
 * fat_disk.c
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "pico/stdlib.h"
#include "ff.h"
#include "diskio.h"

#include "config.h"
#include "log.h"
#include "debug.h"
#include "fat_disk.h"

static uint8_t disk[DISK_BLOCK_COUNT * DISK_BLOCK_SIZE];

void fat_disk_init(void) {
    BYTE work[FF_MAX_SS];
    FRESULT res = f_mkfs("", 0, work, sizeof(work));
    if (res != FR_OK) {
        debug("error: f_mkfs returned %d", res);
        return;
    }

    if (debug_msc_mount() != 0) {
        debug("error mounting disk");
        return;
    }

    // maximum length: 11 bytes
    f_setlabel("DEBUG DISK");

    FIL file;
    res = f_open(&file, "README.md", FA_CREATE_ALWAYS | FA_WRITE);
    if (res != FR_OK) {
        debug("error: f_open returned %d", res);
    } else {
        char readme[1024];
        size_t pos = 0;
        pos += snprintf(readme + pos, 1024 - pos, "# Trackball\r\n");
        pos += snprintf(readme + pos, 1024 - pos, "\r\n");
        pos += snprintf(readme + pos, 1024 - pos, "Project by Thomas Buck <thomas@xythobuz.de>\r\n");
        pos += snprintf(readme + pos, 1024 - pos, "Licensed under GPLv3.\r\n");
        pos += snprintf(readme + pos, 1024 - pos, "Source at https://git.xythobuz.de/thomas/Trackball\r\n");

        size_t len = strlen(readme);
        UINT bw;
        res = f_write(&file, readme, len, &bw);
        if ((res != FR_OK) || (bw != len)) {
            debug("error: f_write returned %d", res);
        }

        res = f_close(&file);
        if (res != FR_OK) {
            debug("error: f_close returned %d", res);
        }
    }

    if (debug_msc_unmount() != 0) {
        debug("error unmounting disk");
    }
}

uint8_t *fat_disk_get_sector(uint32_t sector) {
    return disk + (sector * DISK_BLOCK_SIZE);
}

/*
 * FatFS ffsystem.c
 */

void* ff_memalloc(UINT msize) {
    return malloc((size_t)msize);
}

void ff_memfree(void* mblock) {
    free(mblock);
}

/*
 * FatFS diskio.c
 */

DSTATUS disk_status(BYTE pdrv) {
    if (pdrv != 0) {
        debug("invalid drive number %d", pdrv);
        return STA_NODISK;
    }

    return 0;
}

DSTATUS disk_initialize(BYTE pdrv) {
    if (pdrv != 0) {
        debug("invalid drive number %d", pdrv);
        return STA_NODISK;
    }

    return 0;
}

DRESULT disk_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count) {
    if (pdrv != 0) {
        debug("invalid drive number %d", pdrv);
        return RES_PARERR;
    }

    if ((sector + count) > DISK_BLOCK_COUNT) {
        debug("invalid read ((%lu + %u) > %u)", sector, count, DISK_BLOCK_COUNT);
        return RES_ERROR;
    }

    memcpy(buff, disk + (sector * DISK_BLOCK_SIZE), count * DISK_BLOCK_SIZE);
    return RES_OK;
}

DRESULT disk_write(BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count) {
    if (pdrv != 0) {
        debug("invalid drive number %d", pdrv);
        return RES_PARERR;
    }

    if ((sector + count) > DISK_BLOCK_COUNT) {
        debug("invalid read ((%lu + %u) > %u)", sector, count, DISK_BLOCK_COUNT);
        return RES_ERROR;
    }

    memcpy(disk + (sector * DISK_BLOCK_SIZE), buff, count * DISK_BLOCK_SIZE);
    return RES_OK;
}

DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void *buff) {
    if (pdrv != 0) {
        debug("invalid drive number %d", pdrv);
        return RES_PARERR;
    }

    switch (cmd) {
        case GET_SECTOR_COUNT:
            *((LBA_t *)buff) = DISK_BLOCK_COUNT;
            break;

        case GET_SECTOR_SIZE:
            *((WORD *)buff) = DISK_BLOCK_SIZE;
            break;

        case GET_BLOCK_SIZE:
            *((DWORD *)buff) = 1; // non flash memory media
            break;
    }

    return RES_OK;
}
