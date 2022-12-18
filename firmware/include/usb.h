/*
 * usb.h
 */

#ifndef __USB_H__
#define __USB_H__

enum  {
    BLINK_NOT_MOUNTED = 250,
    BLINK_MOUNTED = 1000,
    BLINK_SUSPENDED = 2500,
};

extern uint32_t blink_interval_ms;

void usb_init(void);
void usb_run(void);

void usb_cdc_write(const char *buf, uint32_t count);

#endif // __USB_H__
