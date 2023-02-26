#include <ESP8266WebServer.h>
#include <FS.h>
#include <ESP8266WiFi.h>

#include <ESP8266mDNS.h>
#include "config.h"


IPAddress local_IP(192, 168, 4, 22);
IPAddress gateway(192, 168, 4, 9);
IPAddress subnet(255, 255, 255, 0);



ESP8266WebServer server (80);

struct wifiConfig {
  String ssid;
  String password;
};


//This function takes the parameters passed in the URL(the x and y coordinates of the joystick)
//and sets the motor speed based on those parameters.
void handleJSData()
{
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

struct wifiConfig get_id_pass(String filepath)
{
  wifiConfig ap;
  //Serial.println(filepath);
  File file = SPIFFS.open(filepath, "r");
  ap.ssid = file.readStringUntil('\n');
  ap.password = file.readStringUntil('\n');
  file.close();
  return ap;
}

void reset_controller()
{
  Serial.println("Reset pressed");
  server.send(200, "text/plain", "");
  ESP.restart();

}

void initGpio()
{

  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);

  pinMode(PWMB, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
}

void writeToFile(String id, String pass)
{ String t_location = "/wifi/temp.txt";
  File temp = SPIFFS.open(SAPfile, "w");
  Serial.print("id length:");
  Serial.println(id.length());


  Serial.print("password length:");
  Serial.println(pass.length());

  if (!temp) {
    Serial.println("Error opening file for writing");
    return;
  }
  else
  {
    temp.println(id);
    //temp.print(',');
    temp.println(pass);
    temp.close();
  }
  //  if (filecheck(SAPfile))
  //  {
  //    SPIFFS.remove(SAPfile);
  //    if (filecheck(t_location)) {
  //
  //      SPIFFS.rename(t_location, SAPfile);
  //    }
  //
  //  }
  SPIFFS.gc();

}

void getSapdata()
{
  Serial.println(server.args());

  String ssid = server.arg("ssid");
  String password = server.arg("password");
  Serial.println(ssid);
  Serial.println(password);
  server.send(200, "text/plain", "");
  writeToFile(ssid, password);

}

void sapHome()
{

  struct wifiConfig wifiSettings;
  String reset_controller = server.arg(0);


  if (reset_controller == "yes")
  {
    server.send(200, "text/plain", "");
    ESP.restart();

  }
}

void beginWebserver(bool mode_flag)
{
  if (mode_flag == 1)
  {
    server.serveStatic("/", SPIFFS, "/home.html");

    //call handleJSData function when this URL is accessed by the js in the html file
    server.on("/home.html", reset_controller);

  }
  else
  {

    server.serveStatic("/", SPIFFS, "/homeap.html");

    //call handleJSData function when this URL is accessed by the js in the html file
    server.on("/homeap.html", reset_controller);

  }
    server.serveStatic("/mode/", SPIFFS, "/mode.html");

    server.serveStatic("/ap_home/", SPIFFS, "/ap_home.html");
    server.serveStatic("/ap_home/view/", SPIFFS, "/displayap.html");

    server.serveStatic("/sap_home/", SPIFFS, "/sap_home.html");
    server.serveStatic("/sap_home/form/", SPIFFS, "/form.html");
    server.serveStatic("/sap_home/view/", SPIFFS, "/displaysap.html");



    server.on("/ap_home/ap_home.html", sapHome);
    server.on("/sap_home/form/form.html", getSapdata);
    server.on("/sap_home/sap_home.html", sapHome);


  server.serveStatic("/control/", SPIFFS, "/joystick.html");

  server.serveStatic("/control/virtualjoystick.js", SPIFFS, "/virtualjoystick.js");
  //call handleJSData function when this URL is accessed by the js in the html file
  server.on("/control/jsData.html", handleJSData);
  server.begin();
}

bool filecheck(String filepath) {
  File check = SPIFFS.open(filepath, "r");
  return check;
}

void STA_mode(struct wifiConfig apSettings)
{

  WiFi.mode(WIFI_OFF);

  Serial.print("SSID:");
  Serial.println(apSettings.ssid);
  Serial.print("PASSWORD:");
  Serial.println(apSettings.password);

  WiFi.mode(WIFI_AP);
  Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");

  //WiFi.setAutoConnect(false);
  //WiFi.setAutoReconnect(false); // if wifi attempts to (re)connect to a previous router it kills the access point


  WiFi.softAP(apSettings.ssid, apSettings.password, 10, false, 1);
  Serial.print("Setting AP ... ");
  Serial.print("AP IP address = ");
  Serial.println(WiFi.softAPIP());
}
void wait_for_dns() {
  while (1) {
    delay(1000);
    Serial.println("[ERROR] MDNS responder did not setup for ap mode ");
  }

}
void set_dns()
{

  if (WiFi.getMode() == WIFI_AP) {
    if (!MDNS.begin("wirelessmd", WiFi.softAPIP())) {
      Serial.println("[ERROR] MDNS responder did not setup for ap mode ");
      wait_for_dns();
    }
    MDNS.addService("http", "tcp", 80);
  }
  else {
    if (!MDNS.begin("wirelessmd")) {
      Serial.println("[ERROR] MDNS responder did not setup STA mode");
      wait_for_dns();
    }
  }
}

void setup()
{
  struct wifiConfig apSettings;
  bool mode_flag = 0;
  int retries = 30;
  WiFi.mode(WIFI_OFF);
  Serial.begin(115200);
  delay(2000);

  if (!SPIFFS.begin())
  {
    Serial.println("SPIFFS Mount failed");
    ESP.restart();
  }
  else
  {
    Serial.println("SPIFFS Mount succesfull");
  }

  if (!filecheck(SAPfile)) {
    Serial.println("failed to open SoftAP chnaging mode to AP");
  }
  else {
    Serial.println("running ESP in SoftAP mode");
    apSettings = get_id_pass(SAPfile);
    Serial.print("connecting ESP to:");
    Serial.print(apSettings.ssid);
    Serial.print("PASSWORD:");
    Serial.print(apSettings.password);

    delay(2000);

    WiFi.mode(WIFI_STA);

    //WiFi.begin(id,pass);     //Connect to your WiFi router
    WiFi.begin(apSettings.ssid.c_str(), apSettings.password.c_str());    //Connect to your WiFi router
    while (WiFi.status() != WL_CONNECTED && retries > 0) {
      delay(1000);
      Serial.print(".");
      retries -= 1;
    }
    Serial.println();
    if (retries == 0) {
      Serial.println("failed to connect to network in SoftAp mode chnaging to ap mode");
    }
    else {
      mode_flag = 1;
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
    }

  }

  if (WiFi.status() !=  WL_CONNECTED) {
    if (!filecheck(APfile) )
    {

      Serial.println("failed to open AP and SAP settings defaulting ssid and password");
      apSettings.ssid = "ESP8266";
      apSettings.password = "Helloworld";
      STA_mode(apSettings);
    }
    else
    {
      apSettings = get_id_pass(APfile);
      STA_mode(apSettings);
    }
  }

  set_dns();
  initGpio();
  beginWebserver(mode_flag);
}

void loop()
{
  MDNS.update();
  server.handleClient();

}
