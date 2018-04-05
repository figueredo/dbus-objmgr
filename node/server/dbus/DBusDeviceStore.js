/* eslint-disable class-methods-use-this */
import _ from 'lodash';
import Device from 'domain/Device';

const OBJECT_MANAGER_INTERFACE_NAME = 'org.freedesktop.DBus.ObjectManager';

const DBUS_INTERFACES = [
  'org.freedesktop.DBus.Peer',
  'org.freedesktop.DBus.Introspectable',
  'org.freedesktop.DBus.Properties',
];

class DBusDeviceStore {
  constructor(path, object, store, service) {
    this.path = path;
    this.object = object;
    this.store = store;
    this.service = service;
    this.devices = [];
  }

  async get() {
    const ownProperties = await this.getObjectInterfacesAndProperties(this.object);
    const devicesProperties = await Promise.all(_.chain(this.devices)
      .map(async device => this.getObjectInterfacesAndProperties(device.object))
      .value());

    return _.reduce(
      devicesProperties,
      _.assign,
      ownProperties,
    );
  }

  async add(address) {
    const device = new Device(address, false);
    await this.store.add(device);
    const dbusDevice = this.service.createDevice(this.path, device);
    this.devices.push(dbusDevice);
    await this.emitInterfacesAdded(dbusDevice);
  }

  async remove(address) {
    await this.store.remove(address);
    const dbusDevice = this.removeDevice(address);
    if (dbusDevice) {
      await this.emitInterfacesRemoved(dbusDevice);
      this.service.destroyDevice(dbusDevice);
    }
  }

  async emitInterfacesAdded(dbusDevice) {
    const interfacesAndProperties = await this.getInterfacesAndProperties(dbusDevice.object);
    const objManagerInterface = this.getObjectManagerInterface();
    objManagerInterface.emitSignal(
      'InterfacesAdded',
      dbusDevice.object.path,
      interfacesAndProperties,
    );
  }

  async emitInterfacesRemoved(dbusDevice) {
    const interfaces = this.getAdvertisedInterfacesNames(dbusDevice.object);
    const objManagerInterface = this.getObjectManagerInterface();
    objManagerInterface.emitSignal(
      'InterfacesRemoved',
      dbusDevice.object.path,
      interfaces,
    );
  }

  getObjectManagerInterface() {
    return _.find(
      this.object.interfaces,
      iface => iface.name === OBJECT_MANAGER_INTERFACE_NAME,
    );
  }

  removeDevice(address) {
    return _.chain(this.devices)
      .remove(dbusDevice => dbusDevice.device.address === address)
      .first()
      .value();
  }

  async getObjectInterfacesAndProperties(object) {
    const interfacesAndProperties = {};
    interfacesAndProperties[object.path] = await this.getInterfacesAndProperties(object);
    return interfacesAndProperties;
  }

  async getInterfacesAndProperties(object) {
    const interfaces = this.getAdvertisedInterfaces(object);
    return _.reduce(
      await Promise.all(_.map(
        interfaces,
        this.mapInterfaceToInterfaceProperties.bind(this),
      )),
      _.assign,
    );
  }

  async mapInterfaceToInterfaceProperties(iface) {
    const ifaceProperties = {};
    ifaceProperties[iface.name] = await this.getInterfaceProperties(iface);
    return ifaceProperties;
  }

  getInterfaceProperties(iface) {
    return new Promise((resolve, reject) => {
      iface.getProperties((err, properties) => {
        if (err) {
          reject(err);
        } else {
          resolve(properties);
        }
      });
    });
  }

  getAdvertisedInterfacesNames(object) {
    return _.map(
      this.getAdvertisedInterfaces(object),
      iface => iface.name,
    );
  }

  getAdvertisedInterfaces(object) {
    return _.reject(
      object.interfaces,
      this.isDBusInterface.bind(this),
    );
  }

  isDBusInterface(iface) {
    return _.includes(DBUS_INTERFACES, iface.name);
  }
}

export default DBusDeviceStore;
