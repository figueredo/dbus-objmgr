import DBusServiceFactory from 'dbus/DBusServiceFactory';
import DeviceStore from 'domain/DeviceStore';

const serviceFactory = new DBusServiceFactory();
const service = serviceFactory.create();
const store = new DeviceStore();
service.createStore(store);
