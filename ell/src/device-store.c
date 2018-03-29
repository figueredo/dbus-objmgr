#include <errno.h>
#include <string.h>

#include "device-store.h"

static bool device_cmp_address(const struct device* device,
	const char *address)
{
	return !strcmp(device->address, address);
}

static struct device *find_device(const struct device_store *this,
	const char *address)
{
	return l_queue_find(this->devices,
		(l_queue_match_func_t) device_cmp_address,
		address);
}

static int add(struct device_store *this, struct device *device)
{
	if (find_device(this, device->address))
		return -EPERM;

	l_queue_push_tail(this->devices, device_ref(device));

	return 0;
}

static void remove(struct device_store *this,
	const char *address)
{
	void *device = find_device(this, address);
	if (!device)
		return;
	
	l_queue_remove(this->devices, device);
	device_unref(device);
}

struct device_store *device_store_new()
{
	struct device_store *this = l_new(struct device_store, 1);
	this->devices = l_queue_new();
	this->add = add;
	this->remove = remove;
	return device_store_ref(this);
}

static void device_store_free(struct device_store *this)
{
	l_queue_destroy(this->devices, (l_queue_destroy_func_t) device_unref);
	l_free(this);
}

struct device_store *device_store_ref(struct device_store *this)
{
	__sync_fetch_and_add(&this->refs, 1);

	return this;
}

void device_store_unref(struct device_store *this)
{
	if (__sync_sub_and_fetch(&this->refs, 1))
		return;

	device_store_free(this);
}
