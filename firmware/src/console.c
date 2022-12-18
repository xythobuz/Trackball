/*
 * console.c
 */

#include <string.h>
#include "pico/stdlib.h"
#include "log.h"
#include "pmw3360.h"
#include "console.h"

#define CNSL_BUFF_SIZE 1024

static char cnsl_line_buff[CNSL_BUFF_SIZE + 1];
static uint32_t cnsl_buff_pos = 0;

static void cnsl_interpret(const char *line) {
    if ((strcmp(line, "help") == 0)
            || (strcmp(line, "h") == 0)
            || (strcmp(line, "?") == 0)) {
        print("Trackball Firmware Usage:");
        print("    help - print this message");
        print("     pmw - print PMW3360 status");
        print("    \\x18 - reset to bootloader");
    } else if (strcmp(line, "pmw") == 0) {
        print_pmw_status();
    } else {
        print("unknown command \"%s\"", line);
    }
}

void cnsl_init(void) {
    cnsl_buff_pos = 0;
    for (int i = 0; i < sizeof(cnsl_line_buff); i++) {
        cnsl_line_buff[i] = '\0';
    }
}

static int32_t cnsl_find_line_end(void) {
    for (int32_t i = 0; i < cnsl_buff_pos; i++) {
        if ((cnsl_line_buff[i] == '\r') || (cnsl_line_buff[i] == '\n')) {
            return i;
        }
    }
    return -1;
}

void cnsl_handle_input(const char *buf, uint32_t len) {
    if ((cnsl_buff_pos + len) > CNSL_BUFF_SIZE) {
        debug("error: console input buffer overflow! %lu > %u", cnsl_buff_pos + len, CNSL_BUFF_SIZE);
        cnsl_init();
    }

    memcpy(cnsl_line_buff + cnsl_buff_pos, buf, len);
    cnsl_buff_pos += len;

    int32_t line_len = cnsl_find_line_end();
    if (line_len < 0) {
        // user has not pressed enter yet
        return;
    }

    // convert line to C-style string
    cnsl_line_buff[line_len] = '\0';

    // TODO handle backspace and other terminal commands?

    cnsl_interpret(cnsl_line_buff);

    // clear string and move following data over
    uint32_t cnt = line_len + 1;
    if (cnsl_line_buff[line_len + 1] == '\n') {
        cnt++;
    }
    memset(cnsl_line_buff, '\0', cnt);
    memmove(cnsl_line_buff, cnsl_line_buff + cnt, sizeof(cnsl_line_buff) - cnt);
    cnsl_buff_pos -= cnt;
}
