import Device from 'domain/Device';

class DBusDeviceStore {
  constructor(path, object, store, service) {
    this.path = path;
    this.object = object;
    this.store = store;
    this.service = service;
    this.devices = [];
  }

  async add(address) {
    const device = new Device(address, false);
    await this.store.add(device);
    const dbusDevice = this.service.createDevice(this.path, device);
    this.devices.push(dbusDevice);
  }

  async remove(address) {
    await this.store.remove(address);
    const dbusDevice = this.findDevice(address);
    if (dbusDevice)
      this.service.destroyDevice(dbusDevice);
  }

  findDevice(address) {
    return this.devices
      .find((dbusDevice) => dbusDevice.device.address === address);
  }
}

export default DBusDeviceStore;