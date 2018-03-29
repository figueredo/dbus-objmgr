#ifndef _DEVICE_STORE_H_
#define _DEVICE_STORE_H_

#include <ell/ell.h>

#include "device.h"

struct device_store {
	int refs;
	struct l_queue *devices;

	int (*add)(struct device_store *, struct device *);
	void (*remove)(struct device_store *, const char *);
};

struct device_store *device_store_new();
struct device_store *device_store_ref(struct device_store *this);
void device_store_unref(struct device_store *this);

#endif /* _DEVICE_STORE_H_ */