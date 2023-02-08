#include <ESP8266WebServer.h>
#include <FS.h>
#include <ESP8266WiFi.h>
#include "config.h"

//Define the pins
int STBY = 12; //standby

ESP8266WebServer server (80);

//This function takes the parameters passed in the URL(the x and y coordinates of the joystick)
//and sets the motor speed based on those parameters.
void handleJSData() {
  boolean yDir;
  int x = server.arg(0).toInt() * 10;
  int y = server.arg(1).toInt() * 10;
  int aSpeed = abs(y);
  int bSpeed = abs(y);
  //set the direction based on y being negative or positive
  if ( y < 0 ) {
    yDir = 0;
  }
  else {
    yDir = 1;
  }
  //adjust to speed of each each motor depending on the x-axis
  //it slows down one motor and speeds up the other proportionately
  //based on the amount of turning
  aSpeed = constrain(aSpeed + x / 2, 0, 1023);
  bSpeed = constrain(bSpeed - x / 2, 0, 1023);

  //use the speed and direction values to turn the motors
  //if either motor is going in reverse from what is expected,
  //just change the 2 digitalWrite lines for both motors:
  //!ydir would become ydir, and ydir would become !ydir


  Serial.print("x=");
  Serial.print(aSpeed);
  Serial.print("y=");
  Serial.println(bSpeed);

  //MotorA
  digitalWrite(AIN1, !yDir);
  digitalWrite(AIN2, yDir);
  analogWrite(PWMA, aSpeed);
  //MotorB
  digitalWrite(BIN1, !yDir);
  digitalWrite(BIN2, yDir);
  analogWrite(PWMB, bSpeed);

  //return an HTTP 200
  server.send(200, "text/plain", "");
}

void initGpio()
{
  pinMode(STBY, OUTPUT);

  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);

  pinMode(PWMB, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
}

void beginWebserver()
{

  //server.serveStatic("/", SPIFFS, "/home.html");
  //server.serveStatic("/sap/", SPIFFS, "/sap.html");

  //call handleJSData function when this URL is accessed by the js in the html file
  //server.on("/home.html", reset_controller);
  //server.on("/action_page", getSapdata);


  //server.serveStatic("/control/", SPIFFS, "/joystick.html");
  //server.serveStatic("/control/virtualjoystick.js", SPIFFS, "/virtualjoystick.js");
  //call handleJSData function when this URL is accessed by the js in the html file
  //server.on("/control/jsData.html", handleJSData)


  //set the static pages on SPIFFS for the html and js
  server.serveStatic("/", SPIFFS, "/joystick.html");
  server.serveStatic("/virtualjoystick.js", SPIFFS, "/virtualjoystick.js");
  //call handleJSData function when this URL is accessed by the js in the html file
  server.on("/jsData.html", handleJSData);
  server.begin();
}



void setup()
{
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  //set the pins as outputs
  
  // Debug console
  //initialize SPIFFS to be able to serve up the static HTML files.
  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS Mount failed");
  }
  else {
    Serial.println("SPIFFS Mount succesfull");
  }
  
  initGpio();
  beginWebserver();
}

void loop()
{
  server.handleClient();
}
