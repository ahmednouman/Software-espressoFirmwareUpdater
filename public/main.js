const { Console } = require('console');
const { app, BrowserWindow, ipcMain, shell } = require('electron');
const path = require('path');
const psTree = require('ps-tree');
const { exec } = require('child_process');
const { checkLatestVersion, updateFirmware, readFirmwareVersionFile } = require('./modules/touchFw');

const { firmware_rev } = readFirmwareVersionFile();

function checkProcessById(pid) {
  const intervalId = setInterval(() => {
    exec('tasklist', (error, stdout, stderr) => {
      if (error) {
        console.error(`exec error: ${error}`);
        return;
      }
      const processList = stdout.trim().split('\r\n').slice(2);
      const matchingProcess = processList.find(process => process.split(/\s+/)[1] === pid);
      if (matchingProcess) {
        console.log(`Flow Process with PID ${pid} found!`);
      }else{
        console.log("Flow exited, updater is quitting")
        app.quit()
      }
    });
  }, 500);
}

function checkProcessByName(name) {
  const intervalId = setInterval(() => {
    exec(`tasklist /FI "IMAGENAME eq ${name}"`, (error, stdout, stderr) => {
      if (error) {
        console.error(`exec error: ${error}`);
        return;
      }
      const processList = stdout.trim().split('\r\n').slice(2);
      if (processList.length > 0) {
        console.log(`Flow Process with name ${name} found!`);
      } else {
        console.log("Flow exited, updater is quitting")
        app.quit()
      }
    });
  }, 500);
}

// const args = process.argv.slice(1);
// console.log("args:", args)
const name = "espressoFlow.exe";
if (name) {
  console.log(`Checking for process with name ${name}...`);
  checkProcessByName(name);
} else {
  console.log('No process name specified!');
}

// const args = process.argv.slice(1);
// console.log("args:", args)
// const pid = args[0];
// if (pid) {
//   console.log(`Checking for process with PID ${pid}...`);
//   checkProcessById(pid);
// } else {
//   console.log('No PID specified!');
// }

let smallWindow;
let bigWindow
let errorWindow

function createWindows(){
  smallWindow = new BrowserWindow({
    width: 403,
    height: 231,
    resizable: false,
    frame: false,
    webPreferences: {
      nodeIntegration: false,
      contextIsolation: true,
      preload: path.join(__dirname, 'preload.js')
    }
  });

  if(!app.isPackaged){
    smallWindow.webContents.openDevTools()
  }
  smallWindow.setMenu(null);
  smallWindow.loadFile('pages/index.html')

  //second window
  bigWindow = new BrowserWindow({
    width: 403,
    height: 400,
    resizable: false,
    frame: false,
    webPreferences: {
      nodeIntegration: false,
      contextIsolation: true,
      preload: path.join(__dirname, 'preload.js')
    }
  });

  bigWindow.hide()

  if(!app.isPackaged){
    bigWindow.webContents.openDevTools()
  }

  bigWindow.setMinimumSize(400, 200)


  bigWindow.setMenu(null);
  bigWindow.loadFile('pages/index_second.html')


  errorWindow = new BrowserWindow({
    width: 412,
    height: 270,
    resizable: false,
    frame: false,
    webPreferences: {
      nodeIntegration: false,
      contextIsolation: true,
      preload: path.join(__dirname, 'preload.js')
    }
  });

  if(!app.isPackaged){
    errorWindow.webContents.openDevTools()
  }

  errorWindow.hide()
  errorWindow.setMenu(null);
  errorWindow.loadFile('pages/index_error.html')

  //ipc handling
  ipcMain.on('go_back_1_clicked', (event, arg) => {
    app.quit()
  });

    ipcMain.on('update_1_clicked', (event, arg) => {
    smallWindow.hide()
    bigWindow.show()
  });

    ipcMain.on('go_back_2_clicked', (event, arg) => {
    bigWindow.hide()
    smallWindow.show()
    
  });

    ipcMain.on('update_2_clicked', (event, arg) => {
    bigWindow.loadFile('pages/index_third.html')
    bigWindow.setSize(403, 377)
  })


  ipcMain.on('go_back_3_clicked', (event, arg) => {
    bigWindow.loadFile('pages/index_second.html')
    bigWindow.setSize(403, 400)
  });

  ipcMain.on('update_3_clicked', (event, arg) => {
    checkLatestVersion().then((result) => {
      console.log(`Latest version check: ${result}`);
      if(!result){
        bigWindow.hide()
        smallWindow.loadFile('pages/index_fourth.html')
        smallWindow.setSize(403, 221)
        smallWindow.show()  
        updateFirmware().then((success) => {
          if (success) {
            smallWindow.webContents.send('update-firmware-rev', firmware_rev);
            smallWindow.loadFile('pages/index_fifth.html');
            
          }else{
            smallWindow.hide()
            errorWindow.show()
          }
        });
      }
      else{
        bigWindow.hide()
        smallWindow.loadFile('pages/index_fifth.html')
        smallWindow.setSize(403, 221)
        smallWindow.show()          
      }
    }).catch((error) => {
      console.error(`Error checking latest version: ${error}`);
    });
  })
  

  ipcMain.on('get-firmware-version', (event) => {
    event.returnValue = firmware_rev;
  });


  ipcMain.on('contact', (event, arg) => {
    shell.openExternal('https://support.espres.so');
    app.quit()
  });

  ipcMain.on('done', (event, arg) => {
    app.quit()
  });
}

app.whenReady().then(() => {
  createWindows();

  app.on('activate', function () {
    if (BrowserWindow.getAllWindows().length === 0) createWindows();
  });

  app.on('browser-window-created', (e, window) => {
    window.setSheetOffset(24);
    window.webContents.executeJavaScript(`
      const dragRegion = document.getElementById('title-bar');
      dragRegion.addEventListener('mousedown', (e) => {
        if (e.button === 0) {
          const { x, y } = window.screen.getCursorScreenPoint();
          const { x: windowX, y: windowY } = window.getBounds();
          const offsetX = x - windowX;
          const offsetY = y - windowY;
          const onMouseMove = (e) => {
            const { x, y } = window.screen.getCursorScreenPoint();
            window.setPosition(x - offsetX, y - offsetY);
          };
          const onMouseUp = () => {
            window.removeEventListener('mousemove', onMouseMove);
            window.removeEventListener('mouseup', onMouseUp);
          };
          window.addEventListener('mousemove', onMouseMove);
          window.addEventListener('mouseup', onMouseUp);
        }
      });
    `);
  });
});

app.on('window-all-closed', function () {
  if (process.platform !== 'darwin') app.quit();
});

