import DBusInterfaceBuilder from 'dbus/DBusInterfaceBuilder';

const DEVICE_STORE_INTERFACE_NAME = 'br.org.cesar.knot.DeviceStore';

class DBusDeviceStoreInterfaceFactory {
  constructor(serviceName) {
    this.serviceName = serviceName;
  }

  async create(object, store) {
    const iface = object.createInterface(DEVICE_STORE_INTERFACE_NAME);
    
    const builder = new DBusInterfaceBuilder(iface, this.serviceName);

    builder.addMethod('Add', [[ String, 'address']], [],
      store.add.bind(store));

    builder.addMethod('Remove', [[ String, 'address' ]], [],
      store.remove.bind(store));

    return builder.build();
  }
}

export default DBusDeviceStoreInterfaceFactory;