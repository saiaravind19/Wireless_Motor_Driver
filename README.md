# Prerequisites Installation
- Make sure you install one of the supported version of Arduino IDE and have ESP8266 core installed.
     - Package is tested with Arduino IDE v 1.8.5  <-> plugin v 0.5.0 so it is recommended to use the same.
- To install [Spiffs files uploader](https://github.com/me-no-dev/arduino-esp32fs-plugin) please follow installation tutorials provided in the tutorial.
- Once the plugging and Arduino IDE are installed successfully move on to next step of  installation of stack to controller.

# Installation
- Clone the repo to the system  and open the project in Arduino IDE.
  -  _Note:Make sure data folder is present in current working directory._
- Use spiffs files uploader to flash the HTML UI to ESP. To upload navigate to Tools>ESP8266 Sketch Data Upload.
- Wait for the files to upload once the files are uploaded, compile and upload the program.
