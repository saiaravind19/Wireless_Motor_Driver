## Prerequisites
- Make sure you install one of the [supported version](https://www.arduino.cc/en/software/OldSoftwareReleases) of Arduino IDE and have ESP8266 core installed.
     - Package is tested with Arduino IDE v 1.8.5  <-> plugin v 0.5.0 so it is recommended to use the same.
- To install [Spiffs files uploader](https://github.com/me-no-dev/arduino-esp32fs-plugin) please follow installation tutorials provided in the tutorial.
- Once the plugging and Arduino IDE are installed successfully move on to next step of  installation of stack to controller.

## Installation
- Clone the repo to the system  and open the project in Arduino IDE.
  -  _**Note:Make sure data folder is present in current working directory.**_
- Use spiffs files uploader to flash the HTML UI to ESP. To upload navigate to Tools>ESP8266 Sketch Data Upload.
- Wait for the files to upload once the files are uploaded, compile and upload the program.
     - _**Note:Once the files are uploaded no need to reupload file each time of code updae.**_ 

## Pinout Connections

| Controller Gpio  | MotorDriver pin|
| ------ | ------ |
|    D1    | EN1       |
|    D5    | IN1       |
|    D6   | IN2       |
|    D2    | EN2       |
|    D7    | IN3       |
|    D8   | IN4       |
|    D3    | L1-led       |
|    D0    | L2-led       |

## Accessing UI
- In case using ESP as Access point(AP mode) connect to the network SSID mentioned in the file **data>wifi>ap.txt** present in the working directory.
- Open your fariout web browser and tyrp WirelessMD.local and the UI pops up in he browser and you are ready to control the motors wirelessely.
