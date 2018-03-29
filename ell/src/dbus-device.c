#include <errno.h>

#include "dbus-device.h"
#include "dbus-service.h"

#define DEVICE_INTERFACE_NAME "br.org.cesar.knot.Device"
#define DEVICE_OBJECT_PATH_FORMAT "%s/dev_%s"

#define DEVICE_INTERFACE_METHOD_PAIR_NAME "Pair"
#define DEVICE_INTERFACE_METHOD_PAIR_SIG_IN ""
#define DEVICE_INTERFACE_METHOD_PAIR_SIG_OUT ""

#define DEVICE_INTERFACE_METHOD_FORGET_NAME "Forget"
#define DEVICE_INTERFACE_METHOD_FORGET_SIG_IN ""
#define DEVICE_INTERFACE_METHOD_FORGET_SIG_OUT ""

#define DEVICE_INTERFACE_PROPERTY_ADDRESS_NAME "Address"
#define DEVICE_INTERFACE_PROPERTY_ADDRESS_SIG "s"

#define DEVICE_INTERFACE_PROPERTY_PAIRED_NAME "Paired"
#define DEVICE_INTERFACE_PROPERTY_PAIRED_SIG "b"

static struct l_dbus_message *create_pair_error_reply(
	struct l_dbus_message *msg, int err)
{
	switch (err) {
		case -EINVAL:
			return l_dbus_message_new_error(msg,
				SERVICE_NAME ".InvalidArguments",
				"Missing public and/org private keys");
			break;
		case -EPERM:
			return l_dbus_message_new_error(msg,
				SERVICE_NAME ".AlreadyExists",
				"Already paired");
			break;
		default:
			return l_dbus_message_new_error(msg,
				SERVICE_NAME ".Unknown",
				"Unknown error");
	}
}

static struct l_dbus_message *on_pair_called(struct l_dbus *bus,
	struct l_dbus_message *msg, void *user_data)
{
	struct device *device = user_data;
	int err;

	// TODO: parse PAIR_SIG_IN arguments
	err = device->pair(device, "", "");

	if (!err) {
		// TODO: emit signal
		return l_dbus_message_new_method_return(msg);
	}

	return create_pair_error_reply(msg, err);
}

static struct l_dbus_message *create_forget_error_reply(
	struct l_dbus_message *msg, int err)
{
	switch (err) {
		case -EBUSY:
			return l_dbus_message_new_error(msg,
				SERVICE_NAME ".InProgress",
				"Can't forget device while operation is in progress");
			break;
		default:
			return l_dbus_message_new_error(msg,
				SERVICE_NAME ".Unknown",
				"Unknown error");
	}
}

static struct l_dbus_message *on_forget_called(struct l_dbus *bus,
	struct l_dbus_message *msg, void *user_data)
{
	struct device *device = user_data;
	int err;

	err = device->forget(device);

	if (!err) {
		// TODO: emit signal
		return l_dbus_message_new_method_return(msg);
	}

	return create_forget_error_reply(msg, err);
}

static bool on_get_address(struct l_dbus *bus,
	struct l_dbus_message *message,
	struct l_dbus_message_builder *builder,
	void *user_data)
{
	struct device *device = user_data;
	l_dbus_message_builder_append_basic(builder, 's', device->address);
	return true;
}

static bool on_get_paired(struct l_dbus *bus,
	struct l_dbus_message *message,
	struct l_dbus_message_builder *builder,
	void *user_data)
{
	struct device *device = user_data;
	l_dbus_message_builder_append_basic(builder, 'b', &device->paired);
	return true;
}

static void setup_device_interface(struct l_dbus_interface *interface)
{
	// TODO: change PAIR_SIG_IN to receive arguments
	l_dbus_interface_method(interface,
		DEVICE_INTERFACE_METHOD_PAIR_NAME,
		0, on_pair_called,
		DEVICE_INTERFACE_METHOD_PAIR_SIG_OUT,
		DEVICE_INTERFACE_METHOD_PAIR_SIG_IN);

	l_dbus_interface_method(interface,
		DEVICE_INTERFACE_METHOD_FORGET_NAME,
		0, on_forget_called,
		DEVICE_INTERFACE_METHOD_FORGET_SIG_OUT,
		DEVICE_INTERFACE_METHOD_FORGET_SIG_IN);

	l_dbus_interface_property(interface,
		DEVICE_INTERFACE_PROPERTY_ADDRESS_NAME,
		0,
		DEVICE_INTERFACE_PROPERTY_ADDRESS_SIG,
		on_get_address,
		NULL);

	l_dbus_interface_property(interface,
		DEVICE_INTERFACE_PROPERTY_PAIRED_NAME,
		0,
		DEVICE_INTERFACE_PROPERTY_PAIRED_SIG,
		on_get_paired,
		NULL);
}

bool dbus_device_register_interface(struct l_dbus *bus)
{
	return l_dbus_register_interface(bus,
			DEVICE_INTERFACE_NAME,
			setup_device_interface,
			NULL, false);
}

struct dbus_device *dbus_device_ref(struct dbus_device *this)
{
	__sync_fetch_and_add(&this->refs, 1);

	return this;
}

static char *create_object_path(const char *root, const char *address)
{
	return l_strdup_printf(DEVICE_OBJECT_PATH_FORMAT, root, address);
}

static bool create_dbus_object(struct l_dbus *bus, const char *path,
	struct device *device)
{
	if (!l_dbus_register_object(bus,
		path,
		device_ref(device),
		(l_dbus_destroy_func_t) device_unref,
		DEVICE_INTERFACE_NAME,
		device,
		NULL)) {
		device_unref(device);
		return false;
	}
	return true;
}

struct dbus_device *dbus_device_new(struct l_dbus *bus, const char *root,
	struct device *device)
{
	struct dbus_device *this;
	this = l_new(struct dbus_device, 1);
	this->device = device_ref(device);
	this->bus = bus;
	this->path = create_object_path(root, device->address);

	if (!create_dbus_object(bus, this->path, this->device)) {
		device_unref(this->device);
		l_free(this->path);
		l_free(this);
		return NULL;
	}

	return dbus_device_ref(this);
}

static bool destroy_dbus_object(struct l_dbus *bus, const char *path)
{
	return l_dbus_unregister_object(bus, path);
}

static void dbus_device_free(struct dbus_device *this)
{
	destroy_dbus_object(this->bus, this->path);
	device_unref(this->device);
	l_free(this->path);
	l_free(this);
}

void dbus_device_unref(struct dbus_device *this)
{
	if (__sync_sub_and_fetch(&this->refs, 1))
		return;

	dbus_device_free(this);
}
