import dbus from 'dbus';

function createDBusError(serviceName, error) {
  return new dbus.Error(`${serviceName}.${error.name}`, error.message);
}

async function executeMethod(serviceName, done, method, ...args) {
  try {
    const result = await method(...args);
    done(null, result);
  } catch (e) {
    const dbusErr = createDBusError(serviceName, e);
    done(dbusErr);
  }
}

class DBusInterfaceBuilder {
  constructor(iface, serviceName) {
    this.iface = iface;
    this.serviceName = serviceName;
  }

  build() {
    this.iface.update();
    return this.iface;
  }

  addProperty(name, type, getter, setter) {
    this.iface.addProperty(name, {
      type: dbus.Define(type),
      getter: getter ? this.createGetter(getter) : undefined,
      setter: setter ? this.createSetter(setter) : undefined
    });
    return this;
  }

  addMethod(name, inputTypes, outputType, method) {
    const dbusInputTypes = inputTypes.map(this.createSignature.bind(this));
    const dbusOutputType = this.createSignature(outputType);
    
    this.iface.addMethod(name, {
      in: dbusInputTypes,
      out: dbusOutputType
    },
    this.createMethod(method));
    return this;
  }

  addSignal(name, types) {
    types = types.map(this.createSignature.bind(this));
    this.iface.addSignal(name, {
      types: types
    });
    return this;
  }

  createGetter(getterImpl) {
    return async (done) => {
      await executeMethod(this.serviceName, done, getterImpl);
    };
  }
  
  createSetter(setterImpl) {
    return async (value, done) => {
      await executeMethod(this.serviceName, done, setterImpl, value);
    };
  }

  createMethod(methodImpl) {
    return async (...args) => {
      const done = args.pop();
      await executeMethod(this.serviceName, done, methodImpl, ...args);
    };
  }

  createSignature(type) {
    if (Array.isArray(type)) {
      return type.length > 0 ? dbus.Define(...type) : undefined;
    }

    return type;
  }
}

export default DBusInterfaceBuilder;