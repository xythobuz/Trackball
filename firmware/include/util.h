/*
 * util.h
 */

#ifndef __UTIL_H__
#define __UTIL_H__

void heartbeat_init(void);
void heartbeat_run(void);

int32_t convert_two_complement(int32_t b);

bool str_startswith(const char *str, const char *start);

void reset_to_bootloader(void);

#endif // __UTIL_H__
