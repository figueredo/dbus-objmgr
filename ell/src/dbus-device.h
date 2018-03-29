#ifndef _DBUS_DEVICE_H_
#define _DBUS_DEVICE_H_

#include <ell/ell.h>

#include "device.h"

struct dbus_device {
	int refs;
	struct device *device;
  struct l_dbus *bus;
  char *path;
};

bool dbus_device_register_interface(struct l_dbus *bus);

struct dbus_device *dbus_device_new(struct l_dbus *bus, const char *root,
  struct device *device);
struct dbus_device *dbus_device_ref(struct dbus_device *this);
void dbus_device_unref(struct dbus_device *this);

#endif /* _DBUS_DEVICE_H_ */