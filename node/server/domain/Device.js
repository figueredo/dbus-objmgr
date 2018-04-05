class Device {
  constructor(address, paired) {
    this._address = address;
    this._paired = paired;
  }

  get address() {
    return this._address;
  }

  get paired() {
    return this._paired;
  }

  async pair() {
    this._paired = true;
  }

  async forget() {
    this._paired = false;
  }
}

export default Device;