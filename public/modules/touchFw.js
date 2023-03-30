const { exec } = require('child_process');
const { app } = require('electron');
const path = require('path')
const fs = require('fs');


let filesPath_raw = path.join(process.resourcesPath, 'firmwareUpdater');
if (!app.isPackaged){
  filesPath_raw = ".\\src\\backend\\firmwareUpdater"
  //filesPath_raw = "C:\\Program Files\\espressoFlow\\resources\\firmwareUpdater\\resources\\firmwareUpdater"
}

let filesPath = filesPath_raw

let getfile_version = path.join(filesPath, 'PipeTest2Admin.exe f');
let getfirmware_version = path.join(filesPath, 'PipeTest2Admin.exe v');
let update_version = path.join(filesPath, 'PipeTest2Admin.exe u');

if (filesPath_raw.includes(" ")) {
  getfile_version = `"${path.join(filesPath, 'PipeTest2Admin.exe')}" f`;
  getfirmware_version = `"${path.join(filesPath, 'PipeTest2Admin.exe')}" v`;
  update_version = `"${path.join(filesPath, 'PipeTest2Admin.exe')}" u`;
}

if (filesPath_raw.includes(" ")){
  filesPath = `"${filesPath_raw}"`;
}

function runCommandAndGetLogs(command) {
  console.log(command)
  return new Promise((resolve, reject) => {
    exec(command, (error, stdout, stderr) => {
      if (error) {
        console.error(`Command '${command}' exited with error ${error}`);
        console.error(stderr.trim());
        reject(new Error(`Command '${command}' exited with error ${error}`));
      } else {
        resolve(stdout.trim());
      }
    });
  });
}

function parseFirmwareVersion(logs) {
  const lines = logs.split('\n');
  const usb_version_display = lines.find(line => line.startsWith('Usb_Version=')).split('=')[1];
  const touch_version_display = lines.find(line => line.startsWith('Touch_Version=')).split('=')[1];
  const config_version_display = lines.find(line => line.startsWith('Config_Version=')).split('=')[1];
  return { usb_version_display, touch_version_display, config_version_display };
}

function parseFirmwareConfFile(logs) {
  const lines = logs.split('\n');
  const usb_version_file = lines.find(line => line.startsWith('Usb_Version=')).split('=')[1];
  const touch_version_file = lines.find(line => line.startsWith('Touch_Version=')).split('=')[1];
  const config_version_file = lines.find(line => line.startsWith('Config_Version=')).split('=')[1];
  return { usb_version_file, touch_version_file, config_version_file };
}

function readFirmwareVersionFile() {
  const filePath = path.join(filesPath_raw, 'FirmwareVersion.conf');
  const content = fs.readFileSync(filePath, 'utf-8');
  const lines = content.split('\n');
  const firmware_rev = lines[0].split('=')[1];
  const usb_version_file = lines.find(line => line.startsWith('Usb_Version=')).split('=')[1];
  const touch_version_file = lines.find(line => line.startsWith('Touch_Version=')).split('=')[1];
  const config_version_file = lines.find(line => line.startsWith('Config_Version=')).split('=')[1];
  return { firmware_rev, usb_version_file, touch_version_file, config_version_file };
}

async function checkLatestVersion(){
  try {
    const { usb_version_file, touch_version_file, config_version_file } = readFirmwareVersionFile();
    console.log(`USB Version (File): ${usb_version_file}`);
    console.log(`Touch Version (File): ${touch_version_file}`);
    console.log(`Config Version (File): ${config_version_file}`);

    const logs = await runCommandAndGetLogs(getfirmware_version);
    const { usb_version_display, touch_version_display, config_version_display } = parseFirmwareVersion(logs);
    console.log(`USB Version: ${usb_version_display}`);
    console.log(`Touch Version: ${touch_version_display}`);
    console.log(`Config Version: ${config_version_display}`);

    const result = usb_version_file === usb_version_display &&
      touch_version_file === touch_version_display &&
      config_version_file === config_version_display;

    return result;
  } catch (error) {
    throw error;
  }
}

async function updateFirmware() {
  try {
    const logsPromise = runCommandAndGetLogs(update_version);
    const logs = await Promise.race([logsPromise, new Promise(resolve => setTimeout(resolve, 60000))]);
    if (!logs) {
      console.log("Update process timed out.");
      return false;
    }

    const lines = logs.split('\n');
    const update_status = lines[0];
    const usb_version_updated = lines.find(line => line.startsWith('Usb_Version=')).split('=')[1];
    const touch_version_updated = lines.find(line => line.startsWith('Touch_Version=')).split('=')[1];
    const config_version_updated = lines.find(line => line.startsWith('Config_Version=')).split('=')[1];

    console.log(`Update Status: ${update_status}`);
    console.log(`USB Version (Updated): ${usb_version_updated}`);
    console.log(`Touch Version (Updated): ${touch_version_updated}`);
    console.log(`Config Version (Updated): ${config_version_updated}`);

    const { usb_version_file, touch_version_file, config_version_file } = readFirmwareVersionFile();
    console.log(`USB Version (File): ${usb_version_file}`);
    console.log(`Touch Version (File): ${touch_version_file}`);
    console.log(`Config Version (File): ${config_version_file}`);

    let result = usb_version_file === usb_version_updated &&
      touch_version_file === touch_version_updated &&
      config_version_file === config_version_updated;
    
    if ((update_status.toLowerCase().includes('upgrade success')) && result){
      result = true;
    }else{
      result = false;
    }
    console.log(result)
    return result;
  } catch (error) {
    throw error;
  }
}


module.exports = { checkLatestVersion, updateFirmware, readFirmwareVersionFile };


