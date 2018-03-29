#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ell/ell.h>

#include "dbus-service.h"
#include "dbus-device-store.h"

const char *address[] =
{
	"010203040506",
	"111213141516",
	NULL
};

struct l_dbus *bus = NULL;
struct dbus_device_store *dbus_store = NULL;

static struct dbus_device_store *create_dbus_store(struct l_dbus *bus)
{
	struct dbus_device_store *dbus_store;
	struct device_store *store;

	store = device_store_new();
	dbus_store = dbus_device_store_new(store, bus);
	device_store_unref(store);

	return dbus_store;
}

static void on_service_started(struct l_dbus *service_bus)
{
	int i, err;
	
	if (!service_bus) {
		fprintf(stderr, "Failed to start the D-Bus service\n");
		return;
	}
	bus = service_bus;

	dbus_store = create_dbus_store(bus);
	if (!dbus_store) {
		fprintf(stderr, "Failed to create the device store\n");
		return;
	}

	for (i = 0; address[i]; i++) {
		err = dbus_store->add(dbus_store, address[i]);
		if (err) {
			fprintf(stderr, "Failed to add device (%s): %s\n",
				address[i], strerror(-err));
		}
	}

	fprintf(stdout, "Service started\n");
}

static void stop_service()
{
	if (!dbus_store)
		return;

	dbus_device_store_unref(dbus_store);

	// TODO: destroy bus
}

int main()
{
	if (!l_main_init())
		return -1;

	dbus_service_start(on_service_started);

	l_main_run();

	stop_service();

	l_main_exit();
	
	return 0;
}
