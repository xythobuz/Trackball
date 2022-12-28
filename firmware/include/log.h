/*
 * log.h
 */

#ifndef __LOG_H__
#define __LOG_H__

// for output that is stored in the debug log.
// will be re-played from buffer when terminal connects
#define debug(fmt, ...) debug_log(true, \
        "%08lu %s: " fmt "\r\n", \
        to_ms_since_boot(get_absolute_time()), \
        __func__, \
        ##__VA_ARGS__)

// for interactive output. is not stored or re-played.
#define print(fmt, ...) debug_log(false, fmt, ##__VA_ARGS__)
#define println(fmt, ...) debug_log(false, fmt "\r\n", ##__VA_ARGS__)

void debug_log(bool log, const char *format, ...) __attribute__((format(printf, 2, 3)));

void log_dump_to_usb(void);
void log_dump_to_disk(void);

#endif // __LOG_H__
