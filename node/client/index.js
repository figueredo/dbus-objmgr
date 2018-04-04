import dbus from 'dbus';
import util from 'util';
import _ from 'lodash';

const SERVICE_NAME = 'br.org.cesar.knot';
const OBJ_MANAGER_PATH = '/';
const OBJ_MANAGER_INTERFACE_NAME = 'org.freedesktop.DBus.ObjectManager';
const DEVICE_INTERFACE_NAME = 'br.org.cesar.knot.Device';

const filterDevices = function filterDevices(objects) {
  return _.pickBy(
    objects,
    object => _.has(object, DEVICE_INTERFACE_NAME),
  );
};

const bus = dbus.getBus('system');
bus.getInterface(SERVICE_NAME, OBJ_MANAGER_PATH, OBJ_MANAGER_INTERFACE_NAME, (err, iface) => {
  if (err) {
    console.error(`Failed to get '${OBJ_MANAGER_INTERFACE_NAME}' on '${OBJ_MANAGER_PATH}'`, err);
    bus.disconnect();
    return;
  }

  iface.GetManagedObjects((getManagedObjectsErr, objects) => {
    if (getManagedObjectsErr) {
      console.error('Failed to get managed objects', getManagedObjectsErr);
      bus.disconnect();
      return;
    }

    const devices = filterDevices(objects);
    console.log(util.inspect(devices));
  });
});
