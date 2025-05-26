#ifndef UDEV_UTILS_H
#define UDEV_UTILS_H

/**
 * Listens for the "add" udev events for a specifc USB device
 * 
 * @param vendor_id The value of the ATTRS{idVendor} of the USB device
 * @param product_id The value of the ATTRS{idProduct} of the USB device
 */
void listen_for_device_add_events(const char *vendor_id, const char *product_id);
#endif

