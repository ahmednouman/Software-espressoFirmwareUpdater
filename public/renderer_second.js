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

const goBackBtn_2 = document.getElementById('go_back_2');
  goBackBtn_2.addEventListener('click', () => {
  electron.send('go_back_2_clicked');
});

const update_2 = document.getElementById('update_2');
  update_2.addEventListener('click', () => {
  electron.send('update_2_clicked');
  
});