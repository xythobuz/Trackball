/*
 * usb_hid.h
 */

#ifndef __USB_HID_H__
#define __USB_HID_H__

struct usb_hid_mouse {
    int32_t delta_x;
    int32_t delta_y;
};

void hid_task(void);

#endif // __USB_HID_H__
