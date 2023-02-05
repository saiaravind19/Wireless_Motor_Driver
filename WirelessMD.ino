#include <ESP8266WebServer.h>
#include <FS.h>
#include <ESP8266WiFi.h>

#include <ESP8266mDNS.h>


#define APfile "/wifi/ap.txt"
#define SAPfile "/wifi/sap.txt"




IPAddress local_IP(192, 168, 4, 22);
IPAddress gateway(192, 168, 4, 9);
IPAddress subnet(255, 255, 255, 0);

//Define the pins
int STBY = 12; //standby
//Motor A
int PWMA = 5; //Speed control
int AIN1 = 14; //Direction
int AIN2 = 16; //Direction
//Motor B
int PWMB = 4; //Speed control
int BIN1 = 15; //Direction
int BIN2 = 13; //Direction

ESP8266WebServer server (80);


struct wifiConfig {
  String ssid;
  String password;
};



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
  digitalWrite(STBY, HIGH);
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


struct wifiConfig get_SAPMODE(String filepath)
{ wifiConfig ap;
  //Serial.println(filepath);
  File file = SPIFFS.open(filepath, "r");
  ap.ssid = file.readStringUntil('\n');
  ap.password = file.readStringUntil('\n');
  file.close();
  return ap;
}

void home(){
  Serial.println("button pressed");
  server.send(200, "text/plain", );

}
void setup()
{ struct wifiConfig apSettings;

  String ssid = "ESP8266";
  String password = "123446576543";
  Serial.begin(115200);
  delay(2000);

  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS Mount failed");
  }
  else {
    Serial.println("SPIFFS Mount succesfull");
  }

  File ap = SPIFFS.open(APfile, "r");
  if (!ap) {
    Serial.println("failed to open SoftAP settings defaulting ssid and password");
    apSettings.ssid = "Aravind";
    apSettings.password = "helloboys1";
  }
  else {
    apSettings = get_SAPMODE(APfile);
    Serial.println("SPIFFS Mount succesfull");
    delay(2000);
  }

  Serial.print("SSID:");
  Serial.println(apSettings.ssid);
  Serial.print("PASSWORD:");
  Serial.println(apSettings.password);

 


  Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");
  WiFi.softAP(apSettings.ssid, apSettings.password);
  Serial.print("Setting soft-AP ... ");
  Serial.print("Soft-AP IP address = ");
  Serial.println(WiFi.softAPIP());


  /*
    WiFi.begin(apSettings.ssid, apSettings.password);     //Connect to your WiFi router
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }*/


  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP

  //set the pins as outputs
  pinMode(STBY, OUTPUT);

  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);

  pinMode(PWMB, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  // Debug console
  //initialize SPIFFS to be able to serve up the static HTML files.

  //  Serial.println("WiFi connected");
  //  Serial.println("IP address: ");
  //  Serial.println(WiFi.localIP());


  //set the static pages on SPIFFS for the html and js


  server.serveStatic("/", SPIFFS, "/home.html");
  //call handleJSData function when this URL is accessed by the js in the html file
  server.on("/home.html", home);



  server.serveStatic("/control/", SPIFFS, "/joystick.html");
  server.serveStatic("/control/virtualjoystick.js", SPIFFS, "/virtualjoystick.js");
  //call handleJSData function when this URL is accessed by the js in the html file
  server.on("/control/jsData.html", handleJSData);
  server.begin();


}

void loop()
{  
  server.handleClient();
}
