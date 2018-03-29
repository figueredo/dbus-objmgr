#ifndef _DBUS_DEVICE_STORE_H_
#define _DBUS_DEVICE_STORE_H_

#include <ell/ell.h>

#include "device-store.h"

struct dbus_device_store {
	int refs;
	struct device_store *store;
	struct l_dbus *bus;
	char *path;
	struct l_hashmap *address_device_map;

	int (*add)(struct dbus_device_store *, const char *);
	void (*remove)(struct dbus_device_store *, const char *);
};

bool dbus_device_store_register_interface(struct l_dbus *bus);

struct dbus_device_store *dbus_device_store_new(
	struct device_store *store, struct l_dbus *bus);
struct dbus_device_store *dbus_device_store_ref(
	struct dbus_device_store *this);
void dbus_device_store_unref(struct dbus_device_store *this);

#endif /* _DBUS_DEVICE_STORE_H_ */