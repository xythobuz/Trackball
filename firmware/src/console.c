/*
 * console.c
 */

#include <inttypes.h>
#include <string.h>
#include "pico/stdlib.h"

#include "config.h"
#include "log.h"
#include "pmw3360.h"
#include "util.h"
#include "console.h"

#define CNSL_BUFF_SIZE 1024

static char cnsl_line_buff[CNSL_BUFF_SIZE + 1];
static uint32_t cnsl_buff_pos = 0;

static char cnsl_last_command[CNSL_BUFF_SIZE + 1];

static char cnsl_repeated_command[CNSL_BUFF_SIZE + 1];
static bool repeat_command = false;
static uint32_t last_repeat_time = 0;

static void cnsl_interpret(const char *line) {
    if (strlen(line) == 0) {
        // repeat last command once
        print("repeating command \"%s\"", cnsl_last_command);
        cnsl_interpret(cnsl_last_command);
        print();
        return;
    } else if (strcmp(line, "repeat") == 0) {
        if (!repeat_command) {
            // mark last command to be repeated multiple times
            strncpy(cnsl_repeated_command, cnsl_last_command, CNSL_BUFF_SIZE + 1);
            last_repeat_time = to_ms_since_boot(get_absolute_time()) - 1001;
            repeat_command = true;
        } else {
            // stop repeating
            repeat_command = false;
        }
    } else if ((strcmp(line, "help") == 0)
            || (strcmp(line, "h") == 0)
            || (strcmp(line, "?") == 0)) {
        print("Trackball Firmware Usage:");
        print("    cpi - print current sensitivity");
        print("  cpi N - set sensitivity");
        print("   pmws - print PMW3360 status");
        print("   pmwr - reset PMW3360");
        print("  reset - reset back into this firmware");
        print("   \\x18 - reset to bootloader");
        print(" repeat - repeat last command once per second");
        print("   help - print this message");
        print("Press Enter with no input to repeat last command.");
        print("Use repeat to continuously execute last command.");
        print("Stop this by calling repeat again.");
    } else if (strcmp(line, "pmws") == 0) {
        pmw_print_status();
    } else if (strcmp(line, "pmwr") == 0) {
        print("user requests re-initializing of PMW3360");
        int r = pmw_init();
        if (r < 0) {
            print("error initializing PMW3360");
        } else {
            print("PMW3360 re-initialized successfully");
        }
    } else if (strcmp(line, "cpi") == 0) {
        uint8_t sense = pmw_get_sensitivity();
        uint16_t cpi = PMW_SENSE_TO_CPI(sense);
        print("current cpi: %u (0x%02X)", cpi, sense);
    } else if (str_startswith(line, "cpi ")) {
        const char *num_str = line + 4;
        uintmax_t num = strtoumax(num_str, NULL, 10);
        if ((num < 100) || (num > 12000)) {
            print("invalid cpi %llu, needs to be %u <= cpi <= %u", num, 100, 12000);
        } else {
            num /= 100;
            num -= 1;
            print("setting cpi to 0x%02llX", num);
            pmw_set_sensitivity(num);
        }
    } else if (strcmp(line, "reset") == 0) {
        reset_to_main();
    } else {
        print("unknown command \"%s\"", line);
        return;
    }

    print();
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

void cnsl_run(void) {
    if (repeat_command) {
        uint32_t now = to_ms_since_boot(get_absolute_time());
        if (now >= (last_repeat_time + 1000)) {
            print("repeating command \"%s\"", cnsl_repeated_command);
            cnsl_interpret(cnsl_repeated_command);
            print();

            last_repeat_time = now;
        }
    }
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

    // store command for eventual repeats
    strncpy(cnsl_last_command, cnsl_line_buff, CNSL_BUFF_SIZE + 1);

    // clear string and move following data over
    uint32_t cnt = line_len + 1;
    if (cnsl_line_buff[line_len + 1] == '\n') {
        cnt++;
    }
    memset(cnsl_line_buff, '\0', cnt);
    memmove(cnsl_line_buff, cnsl_line_buff + cnt, sizeof(cnsl_line_buff) - cnt);
    cnsl_buff_pos -= cnt;
}
