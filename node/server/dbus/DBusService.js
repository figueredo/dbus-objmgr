import DBusDeviceStore from 'dbus/DBusDeviceStore';
import DBusDevice from 'dbus/DbusDevice';

let nextIndex = 0;

class DBusService {
  constructor(service, serviceName, storeInterfaceFactory, deviceInterfaceFactory) {
    this.service = service;
    this.serviceName = serviceName;
    this.storeInterfaceFactory = storeInterfaceFactory;
    this.deviceInterfaceFactory = deviceInterfaceFactory;
  }

  createStore(store) {
    const path = this.createStorePath();
    const object = this.service.createObject(path);
    const dbusStore = new DBusDeviceStore(path, object, store, this);
    this.storeInterfaceFactory.create(object, dbusStore);
    return dbusStore;
  }

  destroyStore(dbusStore) {
    this.service.removeObject(dbusStore.object);
  }

  createDevice(rootPath, device) {
    const path = this.createDevicePath(rootPath, device);
    const object = this.service.createObject(path);
    this.deviceInterfaceFactory.create(object, device);
    return new DBusDevice(path, object, device);
  }

  destroyDevice(dbusDevice) {
    this.service.removeObject(dbusDevice.object);
  }

  createStorePath() {
    return `/store${nextIndex++}`;
  }

  createDevicePath(rootPath, device) {
    return `${rootPath}/dev_${device.address}`;
  }
}

export default DBusService;