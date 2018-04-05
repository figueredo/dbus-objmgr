import DBusInterfaceBuilder from 'dbus/DBusInterfaceBuilder';

const DEVICE_INTERFACE_NAME = 'br.org.cesar.knot.Device';

class DBusDeviceInterfaceFactory {
  constructor(serviceName) {
    this.serviceName = serviceName;
  }

  async create(object, device) {
    const iface = object.createInterface(DEVICE_INTERFACE_NAME);
    
    const builder = new DBusInterfaceBuilder(iface, this.serviceName);
    
    builder.addProperty('Paired',
      Boolean,
      () => device.paired);

    builder.addProperty('Address',
      String,
      () => device.address);

    builder.addMethod('Pair', [], [],
      device.pair.bind(device));

    builder.addMethod('Forget', [], [],
      device.forget.bind(device));

    return builder.build();
  }
}

export default DBusDeviceInterfaceFactory;