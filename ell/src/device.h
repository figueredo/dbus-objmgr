#ifndef _DEVICE_H_
#define _DEVICE_H_

#include <stdbool.h>

struct device {
	int refs;
	bool paired;
	char *address;

	int (*pair)(struct device *, const char *, const char *);
	int (*forget)(struct device *);
};

struct device *device_new(const char *address, bool paired);
struct device *device_ref(struct device *device);
void device_unref(struct device *device);

#endif /* _DEVICE_H_ */