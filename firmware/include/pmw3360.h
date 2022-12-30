/*
 * pmw3360.h
 */

#ifndef __PMW3360_H__
#define __PMW3360_H__

#include <sys/types.h>

struct pmw_motion {
    bool motion;
    int32_t delta_x;
    int32_t delta_y;
};

int pmw_init(void);
void pmw_run(void);
bool pmw_is_alive(void);

struct pmw_motion pmw_get(void);

/*
 * 0x00: 100 cpi (minimum cpi)
 * 0x01: 200 cpi
 * 0x02: 300 cpi
 * ...
 * 0x31: 5000 cpi (default cpi)
 * ...
 * 0x77: 12000 cpi (maximum cpi)
 */
void pmw_set_sensitivity(uint8_t sens);
uint8_t pmw_get_sensitivity(void);
#define PMW_SENSE_TO_CPI(sense) (100 + (sense * 100))
#define PMW_CPI_TO_SENSE(cpi) ((cpi / 100) - 1)

void pmw_set_angle(int8_t angle);
int8_t pmw_get_angle(void);

void pmw_print_status(char *buff, size_t len);
void pmw_dump_data(bool serial);

ssize_t pmw_frame_capture(uint8_t *buff, size_t buffsize);
#define PMW_FRAME_CAPTURE_LEN 1296

#endif // __PMW3360_H__
