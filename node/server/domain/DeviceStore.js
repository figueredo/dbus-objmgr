/* eslint-disable class-methods-use-this */
import _ from 'lodash';
import AlreadyExistsError from 'domain/AlreadyExistsError';

class DeviceStore {
  constructor() {
    this.devices = [];
  }

  async add(device) {
    if (this.exists(device.address)) {
      throw new AlreadyExistsError('Device already exists');
    }

    this.devices.push(device);
  }

  async remove(address) {
    _.remove(
      this.devices,
      device => this.hasSameAddress(device, address),
    );
  }

  exists(address) {
    return _.some(
      this.devices,
      device => this.hasSameAddress(device, address),
    );
  }

  hasSameAddress(device, address) {
    return device.address === address;
  }
}

export default DeviceStore;
