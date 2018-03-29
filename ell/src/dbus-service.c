#include "dbus-service.h"
#include "dbus-device.h"
#include "dbus-device-store.h"

struct dbus_service {
	struct l_dbus *bus;
	void (*on_service_started)(struct l_dbus *);
};

static void on_name_acquired(struct l_dbus *bus, bool success, bool queued,
	void *user_data)
{
	struct dbus_service *service = user_data;

	if (!success
		|| !l_dbus_object_manager_enable(bus)
		|| !dbus_device_register_interface(bus)
		|| !dbus_device_store_register_interface(bus))
	{
		service->on_service_started(NULL);
		// TODO: destroy bus
		return;
	}

	service->on_service_started(bus);
}

static void on_bus_ready(void *user_data)
{
	struct dbus_service *service = user_data;

	l_dbus_name_acquire(service->bus,
		SERVICE_NAME,
		false, false, true,
		on_name_acquired, service);
}

void dbus_service_start(void (*on_service_started)(struct l_dbus *))
{
	struct dbus_service *service = l_new(struct dbus_service, 1);
	service->bus = l_dbus_new_default(L_DBUS_SYSTEM_BUS);
	service->on_service_started = on_service_started;
	l_dbus_set_ready_handler(service->bus, on_bus_ready, service, l_free);
}