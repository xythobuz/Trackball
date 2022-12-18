/*
 * log.h
 */

#ifndef __LOG_H__
#define __LOG_H__

void debug_log(const char *format, ...) __attribute__((format(printf, 1, 2)));

#define debug(fmt, ...) debug_log( \
        "%08u %s: " fmt "\r\n", \
        to_ms_since_boot(get_absolute_time()), \
        __func__, \
        ##__VA_ARGS__)

void log_dump_to_usb(void);

#endif // __LOG_H__
