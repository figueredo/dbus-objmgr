import DBusInterfaceBuilder from 'dbus/DBusInterfaceBuilder';

const OBJECT_MANAGER_INTERFACE_NAME = 'org.freedesktop.DBus.ObjectManager';

class DBusObjectManagerInterfaceFactory {
  constructor(serviceName) {
    this.serviceName = serviceName;
  }

  async create(object, manager) {
    const iface = object.createInterface(OBJECT_MANAGER_INTERFACE_NAME);
    
    const builder = new DBusInterfaceBuilder(iface, this.serviceName);

    builder.addMethod('GetManagedObjects', [], { 
      type: 'a{oa{sa{sv}}}',
      name: 'objpath_interfaces_and_properties'
    }, manager.get.bind(manager));

    builder.addSignal('InterfacesAdded', [
      {
        type: 'o',
        name: 'objpath'
      },
      {
        type: 'a{sa{sv}}',
        name: 'interfaces_and_properties'
      }
    ]);

    builder.addSignal('InterfacesRemoved', [
      {
        type: 'o',
        name: 'objpath'
      },
      {
        type: 'as',
        name: 'interfaces'
      }
    ]);

    return builder.build();
  }
}

export default DBusObjectManagerInterfaceFactory;