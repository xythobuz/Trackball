/*
 * usb_msc.h
 */

#ifndef __USB_MSC_H__
#define __USB_MSC_H__

bool msc_is_medium_available(void);
void msc_set_medium_available(bool state);

#endif // __USB_MSC_H__
