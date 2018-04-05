import _ from 'lodash';
import DBusServiceFactory from 'dbus/DBusServiceFactory';
import Device from 'domain/Device';
import DeviceStore from 'domain/DeviceStore';

const serviceFactory = new DBusServiceFactory();
const service = serviceFactory.create();
const store = new DeviceStore();
const dbusStore = service.createStore(store);
