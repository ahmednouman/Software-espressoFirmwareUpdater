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

const goBackBtn_1 = document.getElementById('go_back_1');
  goBackBtn_1.addEventListener('click', () => {
  electron.send('go_back_1_clicked', 'Hello from renderer process!');
});

const update_1 = document.getElementById('update_1');
  update_1.addEventListener('click', () => {
  electron.send('update_1_clicked', 'Hello update!');
  
});