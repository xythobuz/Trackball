/*
 * pmw3360.c
 *
 * Based on:
 *  - PMW3360 Datasheet
 *  - https://github.com/raspberrypi/pico-examples/blob/master/spi/bme280_spi/bme280_spi.c
 *
 * Pinout:
 *   GPIO 16 (pin 21) MISO -> MISO on PMW3360 board
 *   GPIO 17 (pin 22) CS   -> NCS on PMW3360 board
 *   GPIO 18 (pin 24) SCK  -> SCK on PMW3360 board
 *   GPIO 19 (pin 25) MOSI -> MOSI on PMW3360 board
 *      3.3v (pin 36)      -> VCC on PMW3360 board
 *       GND (pin 38)      -> GND on PMW3360 board
 *
 * NOTE: Ensure the PMW3360 breakout board is capable of being driven at 3.3v NOT 5v.
 * The Pico GPIO (and therefore SPI) cannot be used at 5v.
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"
#include "hardware/watchdog.h"
#include "ff.h"

#include "config.h"
#include "log.h"
#include "util.h"
#include "pmw3360_registers.h"
#include "pmw3360_srom.h"
#include "pmw3360.h"

#define HEALTH_CHECK_INTERVAL_MS 1000

#if !defined(spi_default) || !defined(PICO_DEFAULT_SPI_SCK_PIN) || !defined(PICO_DEFAULT_SPI_TX_PIN) || !defined(PICO_DEFAULT_SPI_RX_PIN) || !defined(PICO_DEFAULT_SPI_CSN_PIN)
#error PMW3360 API requires a board with SPI pins
#endif

static volatile int32_t delta_x = 0, delta_y = 0;
static volatile bool mouse_motion = false;
static uint32_t last_health_check = 0;

#ifdef PMW_IRQ_COUNTERS
static uint64_t pmw_irq_count_all = 0;
static uint64_t pmw_irq_count_motion = 0;
static uint64_t pmw_irq_count_no_motion = 0;
static uint64_t pmw_irq_count_on_surface = 0;
static uint64_t pmw_irq_count_lifted = 0;
static uint64_t pmw_irq_count_run = 0;
static uint64_t pmw_irq_count_rest1 = 0;
static uint64_t pmw_irq_count_rest2 = 0;
static uint64_t pmw_irq_count_rest3 = 0;
#endif // PMW_IRQ_COUNTERS

void pmw_print_status(char *buff, size_t len) {
    size_t pos = 0;

    bool com = pmw_is_alive();
    if (com) {
        pos += snprintf(buff + pos, len - pos, "Communication to PMW3360 is working\r\n");
    } else {
        pos += snprintf(buff + pos, len - pos, "ERROR: can not communicate to PMW3360\r\n");
    }

#ifdef PMW_IRQ_COUNTERS
    pos += snprintf(buff + pos, len - pos, "Interrupt statistics:\r\n");
    pos += snprintf(buff + pos, len - pos, "    pmw_irq_cnt_all = %llu\r\n", pmw_irq_count_all);
    pos += snprintf(buff + pos, len - pos, " pmw_irq_cnt_motion = %llu\r\n", pmw_irq_count_motion);
    pos += snprintf(buff + pos, len - pos, "pmw_irq_cnt_no_move = %llu\r\n", pmw_irq_count_no_motion);
    pos += snprintf(buff + pos, len - pos, "pmw_irq_cnt_surface = %llu\r\n", pmw_irq_count_on_surface);
    pos += snprintf(buff + pos, len - pos, " pmw_irq_cnt_lifted = %llu\r\n", pmw_irq_count_lifted);
    pos += snprintf(buff + pos, len - pos, "    pmw_irq_cnt_run = %llu\r\n", pmw_irq_count_run);
    pos += snprintf(buff + pos, len - pos, "  pmw_irq_cnt_rest1 = %llu\r\n", pmw_irq_count_rest1);
    pos += snprintf(buff + pos, len - pos, "  pmw_irq_cnt_rest2 = %llu\r\n", pmw_irq_count_rest2);
    pos += snprintf(buff + pos, len - pos, "  pmw_irq_cnt_rest3 = %llu\r\n", pmw_irq_count_rest3);
#endif // PMW_IRQ_COUNTERS
}

struct pmw_motion pmw_get(void) {
    struct pmw_motion r;
    r.motion = mouse_motion;
    r.delta_x = 0;
    r.delta_y = 0;

    if (r.motion) {
        gpio_set_irq_enabled(PMW_MOTION_PIN, GPIO_IRQ_LEVEL_LOW, false);

        r.delta_x = delta_x;
        r.delta_y = delta_y;
        delta_x = 0;
        delta_y = 0;

        gpio_set_irq_enabled(PMW_MOTION_PIN, GPIO_IRQ_LEVEL_LOW, true);
    }

    return r;
}

static inline void pmw_cs_select() {
    asm volatile("nop \n nop \n nop");
    gpio_put(PICO_DEFAULT_SPI_CSN_PIN, 0); // Active low
    asm volatile("nop \n nop \n nop");
}

static inline void pmw_cs_deselect() {
    asm volatile("nop \n nop \n nop");
    gpio_put(PICO_DEFAULT_SPI_CSN_PIN, 1);
    asm volatile("nop \n nop \n nop");
}

static void pmw_write_register(uint8_t reg, uint8_t data) {
    pmw_cs_select();

    reg |= WRITE_BIT;
    spi_write_blocking(spi_default, &reg, 1);

    busy_wait_us(15);

    spi_write_blocking(spi_default, &data, 1);

    busy_wait_us(20);
    pmw_cs_deselect();
    busy_wait_us(100);
}

static uint8_t pmw_read_register(uint8_t reg) {
    pmw_cs_select();

    reg &= ~WRITE_BIT;
    spi_write_blocking(spi_default, &reg, 1);

    busy_wait_us(160);

    uint8_t buf = 0;
    spi_read_blocking(spi_default, 0, &buf, 1);

    busy_wait_us(1);
    pmw_cs_deselect();
    busy_wait_us(20);

    return buf;
}

static void pmw_write_register_burst(uint8_t reg, const uint8_t *buf, uint16_t len) {
    pmw_cs_select();

    reg |= WRITE_BIT;
    spi_write_blocking(spi_default, &reg, 1);

    busy_wait_us(15);

    for (uint16_t i = 0; i < len; i++) {
        spi_write_blocking(spi_default, buf + i, 1);
        busy_wait_us(15);
    }

    pmw_cs_deselect();

    busy_wait_us(1);
}

static void pmw_read_register_burst(uint8_t reg, uint8_t *buf, uint16_t len) {
    pmw_cs_select();

    reg &= ~WRITE_BIT;
    spi_write_blocking(spi_default, &reg, 1);

    busy_wait_us(35);

    spi_read_blocking(spi_default, 0, buf, len);

    pmw_cs_deselect();
    busy_wait_us(1);
}

static uint8_t pmw_srom_download(void) {
    // Write 0 to Rest_En bit of Config2 register to disable Rest mode
    pmw_write_register(REG_CONFIG2, 0x00);

    // Write 0x1d to SROM_Enable register for initializing
    pmw_write_register(REG_SROM_ENABLE, 0x1D);

    // Wait for 10 ms
    busy_wait_ms(10);

    // Write 0x18 to SROM_Enable register again to start SROM Download
    pmw_write_register(REG_SROM_ENABLE, 0x18);

    busy_wait_us(120);

    // Write SROM file into SROM_Load_Burst register, 1st data must start with SROM_Load_Burst address.
    pmw_write_register_burst(REG_SROM_LOAD_BURST, pmw_fw_data, pmw_fw_length);

    busy_wait_us(200);

    // Read the SROM_ID register to verify the ID before any other register reads or writes
    uint8_t srom_id = pmw_read_register(REG_SROM_ID);
    return srom_id;
}

static uint8_t pmw_power_up(void) {
    pmw_cs_deselect();
    pmw_cs_select();
    pmw_cs_deselect();

    // Write 0x5A to Power_Up_Reset register
    pmw_write_register(REG_POWER_UP_RESET, 0x5A);

    // Wait for at least 50ms
    busy_wait_ms(50);

    // Read from registers 0x02, 0x03, 0x04, 0x05 and 0x06 one time
    for (uint8_t reg = REG_MOTION; reg <= REG_DELTA_Y_H; reg++) {
        pmw_read_register(reg);
    }

    // Perform SROM download
    uint8_t srom_id = pmw_srom_download();
    return srom_id;
}

static struct pmw_motion_report pmw_motion_read(void) {
    // Write any value to Motion_Burst register
    pmw_write_register(REG_MOTION_BURST, 0x42);

    // Start reading SPI Data continuously up to 12 bytes
    struct pmw_motion_report motion_report;
    pmw_read_register_burst(REG_MOTION_BURST, (uint8_t *)&motion_report, sizeof(motion_report));

    return motion_report;
}

static uint16_t pmw_srom_checksum(void) {
    pmw_write_register(REG_SROM_ENABLE, 0x15);

    // Wait for at least 10 ms
    busy_wait_ms(10);

    uint16_t data = pmw_read_register(REG_DATA_OUT_LOWER);
    data |= pmw_read_register(REG_DATA_OUT_UPPER) << 8;
    return data;
}

static void pmw_spi_init(void) {
    // Use SPI0 at 2MHz
    spi_init(spi_default, 2 * 1000 * 1000);
    gpio_set_function(PICO_DEFAULT_SPI_RX_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);

    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_init(PICO_DEFAULT_SPI_CSN_PIN);
    gpio_set_dir(PICO_DEFAULT_SPI_CSN_PIN, GPIO_OUT);
    gpio_put(PICO_DEFAULT_SPI_CSN_PIN, 1);

    spi_set_format(spi_default,
                   8, // Number of bits per transfer
                   1, // Polarity (CPOL)
                   1, // Phase (CPHA)
                   SPI_MSB_FIRST);

    // make the SPI pins available to picotool
    bi_decl(bi_3pins_with_func(PICO_DEFAULT_SPI_RX_PIN, PICO_DEFAULT_SPI_TX_PIN, PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI));
    bi_decl(bi_1pin_with_name(PICO_DEFAULT_SPI_CSN_PIN, "SPI CS"));
}

static void pmw_handle_interrupt(void) {
    struct pmw_motion_report motion_report = pmw_motion_read();

#ifdef PMW_IRQ_COUNTERS
    pmw_irq_count_all++;

    if (motion_report.motion & (1 << REG_MOTION_MOT)) {
        pmw_irq_count_motion++;
    } else {
        pmw_irq_count_no_motion++;
    }

    if (motion_report.motion & (1 << REG_MOTION_LIFT)) {
        pmw_irq_count_lifted++;
    } else {
        pmw_irq_count_on_surface++;
    }

    if ((motion_report.motion & (1 << REG_MOTION_OP_1))
            && (motion_report.motion & (1 << REG_MOTION_OP_2))) {
        pmw_irq_count_rest3++;
    } else if (motion_report.motion & (1 << REG_MOTION_OP_1)) {
        pmw_irq_count_rest1++;
    } else if (motion_report.motion & (1 << REG_MOTION_OP_2)) {
        pmw_irq_count_rest2++;
    } else {
        pmw_irq_count_run++;
    }
#endif // PMW_IRQ_COUNTERS

    uint16_t delta_x_raw = motion_report.delta_x_l | (motion_report.delta_x_h << 8);
    uint16_t delta_y_raw = motion_report.delta_y_l | (motion_report.delta_y_h << 8);

    delta_x += convert_two_complement(delta_x_raw);
    delta_y += convert_two_complement(delta_y_raw);
    mouse_motion = true;
}

static void pmw_motion_irq(void) {
    if (gpio_get_irq_event_mask(PMW_MOTION_PIN) & GPIO_IRQ_LEVEL_LOW) {
        gpio_acknowledge_irq(PMW_MOTION_PIN, GPIO_IRQ_LEVEL_LOW);
        pmw_handle_interrupt();
    }
}

void pmw_set_sensitivity(uint8_t sens) {
    if (sens > 0x77) {
        debug("invalid sense, clamping (0x%X > 0x77)", sens);
        sens = 0x77;
    }

    pmw_write_register(REG_CONFIG1, sens);
    pmw_write_register(REG_CONFIG5, sens);
}

uint8_t pmw_get_sensitivity(void) {
    uint8_t sense_y = pmw_read_register(REG_CONFIG1);
    uint8_t sense_x = pmw_read_register(REG_CONFIG5);
    if (sense_y != sense_x) {
        debug("sensitivity differs for x (0x%02X) and y (0x%02X). resetting.", sense_x, sense_y);
        pmw_write_register(REG_CONFIG5, sense_y);
    }
    return sense_y;
}

static void pmw_irq_start(void) {
    gpio_set_irq_enabled(PMW_MOTION_PIN, GPIO_IRQ_LEVEL_LOW, true);
}

static void pmw_irq_stop(void) {
    gpio_set_irq_enabled(PMW_MOTION_PIN, GPIO_IRQ_LEVEL_LOW, false);
}

static void pmw_irq_init(void) {
    static bool first_init = false;

    if (!first_init) {
        // setup MOTION pin interrupt to handle reading data
        gpio_add_raw_irq_handler(PMW_MOTION_PIN, pmw_motion_irq);
        irq_set_enabled(IO_IRQ_BANK0, true);
        first_init = true;
    }

    pmw_irq_start();

    // make MOTION pin available to picotool
    bi_decl(bi_1pin_with_name(PMW_MOTION_PIN, "PMW3360 MOTION"));
}

bool pmw_is_alive(void) {
    bool r = true;
    gpio_set_irq_enabled(PMW_MOTION_PIN, GPIO_IRQ_LEVEL_LOW, false);

    uint8_t prod_id = pmw_read_register(REG_PRODUCT_ID);
    uint8_t inv_prod_id = pmw_read_register(REG_INVERSE_PRODUCT_ID);
    if (prod_id != ((~inv_prod_id) & 0xFF)) {
        r = false;
    }

    gpio_set_irq_enabled(PMW_MOTION_PIN, GPIO_IRQ_LEVEL_LOW, true);
    return r;
}

ssize_t pmw_frame_capture(uint8_t *buff, size_t buffsize) {
    if ((buffsize < PMW_FRAME_CAPTURE_LEN) || (buff == NULL)) {
        debug("invalid or too small buffer (%u < %u)", buffsize, PMW_FRAME_CAPTURE_LEN);
        return -1;
    }

    pmw_irq_stop();

    // write 0 to Rest_En bit of Config2 register to disable Rest mode
    pmw_write_register(REG_CONFIG2, 0x00);

    // write 0x83 to Frame_Capture register
    pmw_write_register(REG_FRAME_CAPTURE, 0x83);

    // write 0xC5 to Frame_Capture register
    pmw_write_register(REG_FRAME_CAPTURE, 0xC5);

    // wait for 20ms
    busy_wait_ms(20);

    // continue burst read from Raw_data_Burst register until all 1296 raw data are transferred
    pmw_read_register_burst(REG_RAW_DATA_BURST, buff, PMW_FRAME_CAPTURE_LEN);

    return PMW_FRAME_CAPTURE_LEN;
}

#define PMW_DATA_DUMP_SAMPLES 1000

#define PRINTRAW(fmt, ...) {                             \
    if (serial) {                                        \
        print(fmt, ##__VA_ARGS__);                       \
    } else {                                             \
        int n = snprintf(line, 100, fmt, ##__VA_ARGS__); \
        UINT bw;                                         \
        f_write(&file, line, n, &bw);                    \
    }                                                    \
}

void pmw_dump_data(bool serial) {
    char line[100];
    FIL file;
    if (!serial) {
        FRESULT res = f_open(&file, "pmw_data.csv", FA_CREATE_ALWAYS | FA_WRITE);
        if (res != FR_OK) {
            debug("error: f_open returned %d", res);
            return;
        }
    }

    struct pmw_motion_report buff[PMW_DATA_DUMP_SAMPLES];

    println("Will now capture %u data samples from PMW3360", PMW_DATA_DUMP_SAMPLES);
    println("Move trackball to generate some data!");

    pmw_irq_stop();

    for (size_t i = 0; i < PMW_DATA_DUMP_SAMPLES; i++) {
        // wait until MOTION pin is set
        while (gpio_get(PMW_MOTION_PIN)) {
            watchdog_update();
        }

        buff[i] = pmw_motion_read();
    }

    if (serial) {
        println();
    } else {
        println("Now writing data");
    }

    PRINTRAW("time,motion,observation,delta_x,delta_y,squal,raw_sum,raw_max,raw_min,shutter\r\n");

    for (size_t i = 0; i < PMW_DATA_DUMP_SAMPLES; i++) {
        watchdog_update();

        uint16_t delta_x_raw = buff[i].delta_x_l | (buff[i].delta_x_h << 8);
        uint16_t delta_y_raw = buff[i].delta_y_l | (buff[i].delta_y_h << 8);
        uint16_t shutter_raw = buff[i].shutter_lower | (buff[i].shutter_upper << 8);

        PRINTRAW("%llu,", to_us_since_boot(get_absolute_time()));
        PRINTRAW("%u,", buff[i].motion);
        PRINTRAW("%u,", buff[i].observation);
        PRINTRAW("%ld,", convert_two_complement(delta_x_raw));
        PRINTRAW("%ld,", convert_two_complement(delta_y_raw));
        PRINTRAW("%u,", buff[i].squal);
        PRINTRAW("%u,", buff[i].raw_data_sum);
        PRINTRAW("%u,", buff[i].maximum_raw_data);
        PRINTRAW("%u,", buff[i].minimum_raw_data);
        PRINTRAW("%u\r\n", shutter_raw);
    }

    if (serial) {
        println();
    }

    pmw_irq_start();

    if (!serial) {
        FRESULT res = f_close(&file);
        if (res != FR_OK) {
            debug("error: f_close returned %d", res);
        }
    }
}

int pmw_init(void) {
    // initializing takes a while (~160ms)
    watchdog_update();

    pmw_irq_stop();
    pmw_spi_init();

    uint8_t srom_id = pmw_power_up();
    uint8_t prod_id = pmw_read_register(REG_PRODUCT_ID);
    uint8_t inv_prod_id = pmw_read_register(REG_INVERSE_PRODUCT_ID);
    uint16_t srom_checksum = pmw_srom_checksum();

#ifdef PMW_PRINT_IDS
    uint8_t rev_id = pmw_read_register(REG_REVISION_ID);

    debug("SROM ID: 0x%02X", srom_id);
    debug("Product ID: 0x%02X", prod_id);
    debug("~ Prod. ID: 0x%02X", inv_prod_id);
    debug("Revision ID: 0x%02X", rev_id);
    debug("SROM CRC: 0x%04X", srom_checksum);
#endif // PMW_PRINT_IDS

    if (prod_id != ((~inv_prod_id) & 0xFF)) {
        debug("SPI communication error (0x%02X != ~0x%02X)", prod_id, inv_prod_id);
        return -1;
    }

    if ((srom_id != pmw_fw_id) || (srom_checksum != pmw_fw_crc)) {
        if (srom_id != pmw_fw_id) {
            debug("PMW3360 error: invalid SROM ID (0x%02X != 0x%02X)", srom_id, pmw_fw_id);
        }

        if (srom_checksum != pmw_fw_crc) {
            debug("PMW3360 error: invalid SROM CRC (0x%04X != 0x%04X)", srom_checksum, pmw_fw_crc);
        }

        debug("this may require a power-cycle to fix!");
        return -1;
    }

    // Write 0x00 to Config2 register for wired mouse or 0x20 for wireless mouse design
#ifdef PMW_FEATURE_WIRELESS
    pmw_write_register(REG_CONFIG2, 0x20);
#else // ! PMW_FEATURE_WIRELESS
    pmw_write_register(REG_CONFIG2, 0x00);
#endif // PMW_FEATURE_WIRELESS

    // Set sensitivity for each axis
    pmw_write_register(REG_CONFIG2, pmw_read_register(REG_CONFIG2) | 0x04);
    pmw_set_sensitivity(DEFAULT_MOUSE_SENSITIVITY);

    // Set lift-detection threshold to 3mm (max)
    pmw_write_register(REG_LIFT_CONFIG, 0x03);

    pmw_irq_init();

    return 0;
}

void pmw_run(void) {
    uint32_t now = to_ms_since_boot(get_absolute_time());
    if (now >= (last_health_check + HEALTH_CHECK_INTERVAL_MS)) {
        last_health_check = now;
        if (!pmw_is_alive()) {
            debug("PMW3360 is dead. resetting!");
            reset_to_main();
        }
    }
}
