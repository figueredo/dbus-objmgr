#ifndef _DBUS_SERVICE_H_
#define _DBUS_SERVICE_H_

#include <ell/ell.h>

#define SERVICE_NAME "br.org.cesar.knot"

void dbus_service_start(void (*on_service_started)(struct l_dbus *));

#endif /* _DBUS_SERVICE_H_ */