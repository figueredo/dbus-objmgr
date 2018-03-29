#include <errno.h>
#include <ell/ell.h>

#include "device.h"

static int pair(struct device *this,
	const char *public_key, const char *private_key)
{
	if (!public_key || !private_key)
		return -EINVAL;

	if (this->paired)
		return -EPERM;

	this->paired = true;

	return 0;
}

static int forget(struct device *this)
{
	this->paired = false;

	return 0;
}

struct device *device_new(const char *address, bool paired)
{
	struct device *device = l_new(struct device, 1);

	device->paired = paired;
	device->address = l_strdup(address);
	device->pair = pair;
	device->forget = forget;

	return device_ref(device);
}

static void device_free(struct device *device)
{
	l_free(device->address);
	l_free(device);
}

struct device *device_ref(struct device *device)
{
	__sync_fetch_and_add(&device->refs, 1);

	return device;
}

void device_unref(struct device *device)
{
	if (__sync_sub_and_fetch(&device->refs, 1))
		return;

	device_free(device);
}
