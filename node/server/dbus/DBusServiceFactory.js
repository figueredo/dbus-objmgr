import dbus from 'dbus';
import DBusService from 'dbus/DBusService';
import DBusDeviceStoreInterfaceFactory from 'dbus/DBusDeviceStoreInterfaceFactory';
import DBusDeviceInterfaceFactory from 'dbus/DBusDeviceInterfaceFactory';

const SERVICE_NAME = 'br.org.cesar.knot';

class DBusServiceFactory {
  create() {
    const service = dbus.registerService('system', SERVICE_NAME);
    const storeInterfaceFactory = new DBusDeviceStoreInterfaceFactory(SERVICE_NAME);
    const deviceInterfaceFactory = new DBusDeviceInterfaceFactory(SERVICE_NAME);
    return new DBusService(service,
      SERVICE_NAME,
      storeInterfaceFactory,
      deviceInterfaceFactory);
  }
}

export default DBusServiceFactory;