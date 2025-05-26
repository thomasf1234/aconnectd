#include "udev_utils.h"
#include "alsa_utils.h"
#include <stdio.h>       // For fprintf, printf, stderr
#include <string.h>      // For strcmp, strncmp
#include <libudev.h>     // For udev-related functions
#include <sys/select.h>  // For fd_set, FD_ZERO, FD_SET, select
#include <stddef.h>      // For NULL
#include <ctype.h>       // For isdigit

void listen_for_device_add_events(const char *vendor_id, const char *product_id) {
    struct udev *udev = udev_new();
    if (!udev) {
        fprintf(stderr, "Error: Cannot create udev object.\n");
        return;
    }

    struct udev_monitor *mon = udev_monitor_new_from_netlink(udev, "udev");
    udev_monitor_filter_add_match_subsystem_devtype(mon, "usb", NULL);
    udev_monitor_enable_receiving(mon);

    int fd = udev_monitor_get_fd(mon);

    for (;;) {
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(fd, &fds);

        if (select(fd + 1, &fds, NULL, NULL, NULL) > 0) {
            struct udev_device *dev = udev_monitor_receive_device(mon);
            if (dev) {
                const char *action = udev_device_get_action(dev);
                const char *_vendor_id = udev_device_get_sysattr_value(dev, "idVendor");
                const char *_product_id = udev_device_get_sysattr_value(dev, "idProduct");

                const char *name = udev_device_get_sysname(dev);

                // Check if the device ID matches
                if (_vendor_id && strcmp(_vendor_id, vendor_id) == 0 && _product_id && strcmp(_product_id, product_id) == 0) {
                    // Check if the device name starts with "card" and ends with a digit
                    if (strcmp(action, "add") == 0) {
                        return;
                    }
                }

                udev_device_unref(dev);
            }
        }
    }

    udev_unref(udev);
}

