#include <errno.h>

#include "dbus-device-store.h"
#include "dbus-device.h"
#include "dbus-service.h"

#define DEVICE_STORE_INTERFACE_NAME "br.org.cesar.knot.DeviceStore"
#define DEVICE_STORE_OBJECT_PATH_FORMAT "/store%u"

#define DEVICE_STORE_INTERFACE_METHOD_ADD_NAME "Add"
#define DEVICE_STORE_INTERFACE_METHOD_ADD_SIG_IN "s"
#define DEVICE_STORE_INTERFACE_METHOD_ADD_SIG_IN_PARAM_NAME "address"
#define DEVICE_STORE_INTERFACE_METHOD_ADD_SIG_OUT ""

#define DEVICE_STORE_INTERFACE_METHOD_REMOVE_NAME "Remove"
#define DEVICE_STORE_INTERFACE_METHOD_REMOVE_SIG_IN "s"
#define DEVICE_STORE_INTERFACE_METHOD_REMOVE_SIG_IN_PARAM_NAME "address"
#define DEVICE_STORE_INTERFACE_METHOD_REMOVE_SIG_OUT ""

static int next_id = 0;

static int get_next_id()
{
	return next_id++;
}

static struct l_dbus_message *create_add_error_reply(
	struct l_dbus_message *msg, int err)
{
	switch (err) {
		case -EINVAL:
			return l_dbus_message_new_error(msg,
				SERVICE_NAME ".InvalidArguments",
				"Missing address");
			break;
		case -EPERM:
			return l_dbus_message_new_error(msg,
				SERVICE_NAME ".AlreadyExists",
				"Device already exists");
			break;
		default:
			return l_dbus_message_new_error(msg,
				SERVICE_NAME ".Unknown",
				"Unknown error");
	}
}

static struct l_dbus_message *on_add_called(struct l_dbus *bus,
	struct l_dbus_message *msg, void *user_data)
{
	const char *address;
	struct dbus_device_store *store = user_data;
	int err;

	if (!l_dbus_message_get_arguments(msg,
		DEVICE_STORE_INTERFACE_METHOD_ADD_SIG_IN,
		&address)) {
		return create_add_error_reply(msg, -EINVAL);
	}

	err = store->add(store, address);

	if (!err)
		return l_dbus_message_new_method_return(msg);

	return create_add_error_reply(msg, err);
}

static struct l_dbus_message *create_remove_error_reply(
	struct l_dbus_message *msg, int err)
{
	switch (err) {
		case -EINVAL:
			return l_dbus_message_new_error(msg,
				SERVICE_NAME ".InvalidArguments",
				"Missing address");
			break;
		default:
			return l_dbus_message_new_error(msg,
				SERVICE_NAME ".Unknown",
				"Unknown error");
	}
}

static struct l_dbus_message *on_remove_called(struct l_dbus *bus,
	struct l_dbus_message *msg, void *user_data)
{
	const char *address;
	struct dbus_device_store *store = user_data;

	if (!l_dbus_message_get_arguments(msg,
		DEVICE_STORE_INTERFACE_METHOD_REMOVE_SIG_IN,
		&address)) {
		return create_remove_error_reply(msg, -EINVAL);
	}

	store->remove(store, address);

	return l_dbus_message_new_method_return(msg);
}

static void setup_device_store_interface(struct l_dbus_interface *interface)
{
	l_dbus_interface_method(interface,
		DEVICE_STORE_INTERFACE_METHOD_ADD_NAME,
		0, on_add_called,
		DEVICE_STORE_INTERFACE_METHOD_ADD_SIG_OUT,
		DEVICE_STORE_INTERFACE_METHOD_ADD_SIG_IN,
		DEVICE_STORE_INTERFACE_METHOD_ADD_SIG_IN_PARAM_NAME);

	l_dbus_interface_method(interface,
		DEVICE_STORE_INTERFACE_METHOD_REMOVE_NAME,
		0, on_remove_called,
		DEVICE_STORE_INTERFACE_METHOD_REMOVE_SIG_OUT,
		DEVICE_STORE_INTERFACE_METHOD_REMOVE_SIG_IN,
		DEVICE_STORE_INTERFACE_METHOD_REMOVE_SIG_IN_PARAM_NAME);
}

bool dbus_device_store_register_interface(struct l_dbus *bus)
{
	return l_dbus_register_interface(bus,
			DEVICE_STORE_INTERFACE_NAME,
			setup_device_store_interface,
			NULL, false);
}

static int add(struct dbus_device_store *this, const char *address)
{
	int err;
	char *path;
	struct device *device;
	struct dbus_device *dbus_device;
	struct device_store *store = this->store;

	device = device_new(address, false);

	err = store->add(store, device);
	if (err) {
		device_unref(device);
		return err;
	}

	dbus_device = dbus_device_new(this->bus, this->path, device);
	l_hashmap_insert(this->address_device_map, device->address, dbus_device);
	device_unref(device);

	return 0;
}

static void remove(struct dbus_device_store *this,
	const char *address)
{
	struct dbus_device *dbus_device;
	struct device_store *store = this->store;

	store->remove(store, address);

	dbus_device = l_hashmap_remove(this->address_device_map, address);
	if (dbus_device)
		dbus_device_unref(dbus_device);
}

struct dbus_device_store *dbus_device_store_ref(
	struct dbus_device_store *this)
{
	__sync_fetch_and_add(&this->refs, 1);

	return this;
}

static char *create_object_path(unsigned int id)
{
	return l_strdup_printf(DEVICE_STORE_OBJECT_PATH_FORMAT, id);
}

static bool create_dbus_object(struct l_dbus *bus, const char *path,
	struct dbus_device_store *store)
{
	// The reference D-Bus holds is the this same reference
	// When this object is destroyed, the D-Bus object is destroyed too
	return l_dbus_register_object(bus,
		path,
		store,
		NULL,
		DEVICE_STORE_INTERFACE_NAME,
		store,
		NULL);
}

struct dbus_device_store *dbus_device_store_new(
	struct device_store *store, struct l_dbus *bus)
{
	struct dbus_device_store *this;
	this = l_new(struct dbus_device_store, 1);
	this->store = device_store_ref(store);
	this->bus = bus;
	this->path = create_object_path(get_next_id());
	this->address_device_map = l_hashmap_string_new();
	this->add = add;
	this->remove = remove;

	if (!create_dbus_object(bus, this->path, this)) {
		device_store_unref(this->store);
		l_hashmap_destroy(this->address_device_map,
			(l_hashmap_destroy_func_t) dbus_device_unref);
		l_free(this->path);
		l_free(this);
		return NULL;
	}

	return dbus_device_store_ref(this);
}

static bool destroy_dbus_object(struct l_dbus *bus, const char *path)
{
	return l_dbus_unregister_object(bus, path);
}

static void dbus_device_store_free(struct dbus_device_store *this)
{
	destroy_dbus_object(this->bus, this->path);
	device_store_unref(this->store);
	l_hashmap_destroy(this->address_device_map,
		(l_hashmap_destroy_func_t) dbus_device_unref);
	l_free(this->path);
	l_free(this);
}

void dbus_device_store_unref(struct dbus_device_store *this)
{
	if (__sync_sub_and_fetch(&this->refs, 1))
		return;

	dbus_device_store_free(this);
}
