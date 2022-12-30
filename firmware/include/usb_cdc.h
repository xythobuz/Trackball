/*
 * usb_cdc.h
 */

#ifndef __USB_CDC_H__
#define __USB_CDC_H__

void usb_cdc_write(const char *buf, uint32_t count);
void usb_cdc_set_reroute(bool reroute);

#endif // __USB_CDC_H__
