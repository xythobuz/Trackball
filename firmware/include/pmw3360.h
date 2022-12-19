/*
 * pmw3360.h
 */

#ifndef __PMW3360_H__
#define __PMW3360_H__

int pmw_init(void);

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

void pmw_print_status(void);

#endif // __PMW3360_H__
