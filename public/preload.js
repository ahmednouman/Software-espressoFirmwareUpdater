const { contextBridge, ipcRenderer, getCurrentWindow } = require('electron');

contextBridge.exposeInMainWorld('electron', {
  send: (channel, data) => {
    ipcRenderer.send(channel, data);
  },
  receive: (channel, func) => {
    ipcRenderer.on(channel, (event, ...args) => func(...args));
  },
  getCurrentWindow: () => {
    return getCurrentWindow();
  },
  getFirmwareVersion: () => {
    return ipcRenderer.sendSync('get-firmware-version');
  }
});
