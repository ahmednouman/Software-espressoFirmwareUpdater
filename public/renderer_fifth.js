const { ipcRenderer } = window;

const titleBar = document.getElementById('title-bar');

titleBar.addEventListener('mousedown', (e) => {
  if (e.button === 0) {
    const currentWindow = electron.getCurrentWindow();
    const { x, y } = electron.screen.getCursorScreenPoint();
    const { x: windowX, y: windowY } = currentWindow.getBounds();
    const offsetX = x - windowX;
    const offsetY = y - windowY;

    const onMouseMove = (e) => {
      const { x, y } = electron.screen.getCursorScreenPoint();
      currentWindow.setPosition(x - offsetX, y - offsetY);
    };

    const onMouseUp = () => {
      window.removeEventListener('mousemove', onMouseMove);
      window.removeEventListener('mouseup', onMouseUp);
    };

    window.addEventListener('mousemove', onMouseMove);
    window.addEventListener('mouseup', onMouseUp);
  }
});


const update_3 = document.getElementById('done');
  update_3.addEventListener('click', () => {
  electron.send('done');
  
});

document.addEventListener('DOMContentLoaded', () => {
  const firmware_version = window.electron.getFirmwareVersion();
  const firmwareRev = document.getElementById('firmware-rev');
  firmwareRev.innerText = firmware_version
});




