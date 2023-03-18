/*
 * NodeMCU/ESP8266 act as AP (Access Point) and simplest Web Server
 * to control GPIO (on-board LED)
 * Connect to AP "arduino-er", password = "password"
 * Open browser, visit 192.168.4.1
 */
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include "../generic/common.h"
#define PIN_WIRE_SDA (4) 
#define PIN_WIRE_SCL (5)


static const uint8_t Pin0   = 16; 
static const uint8_t Pin1   = 5; 
static const uint8_t Pin2   = 4; 
static const uint8_t Pin3   = 0; 
static const uint8_t Pin4   = 2; 
static const uint8_t Pin5   = 14; 
static const uint8_t Pin6   = 12; 
static const uint8_t Pin7   = 13; 
static const uint8_t Pin8   = 15; 
static const uint8_t Pin9   = 3; 
static const uint8_t Pin10  = 1;


const char *ssid = "JosephO";
const char *password = "johnbull";
int stateLED = LOW;
String resp = "Empty";
ESP8266WebServer server(80);

void handleRoot() {
    response();
}


void handleDevice1On(){
  Serial.println("Device 1 is ON");
  resp = "Device 1 is now ON";
  digitalWrite(Pin1, HIGH);
  response2();
}

void handleDevice1Off(){
  Serial.println("Device 1 is OFF");
   resp = "Device 1 put OFF";
   digitalWrite(Pin1, LOW);
   response2();
}

void handleDevice2On(){
  Serial.println("Device 2 is ON");
   resp = "Device 2 is now ON";
   digitalWrite(Pin2, HIGH);
   response2();
}

void handleDevice2Off(){
  resp = "Device 2 put OFF";
  digitalWrite(Pin2, LOW);
  Serial.println("Device 2 is OFF");
  response2();
}

void handleDevice3On(){
   resp = "Device 3 is now ON";
   digitalWrite(Pin3, HIGH);
  Serial.println("Device 3 is ON");
  response2();
}

void handleDevice3Off(){
  resp = "Device 3 put OFF";
  digitalWrite(Pin3, LOW);
  Serial.println("Device 3 is OFF");
  response2();
}

void handleDevice4On(){
   resp = "Device 4 is now ON";
   digitalWrite(Pin4, HIGH);
  Serial.println("Device 4 is ON");
   response2();
}

void handleDevice4Off(){
  resp = "Device 4 put OFF";
  digitalWrite(Pin4, LOW);
  Serial.println("Device 4 is OFF");
  response2();
}


void handleLedOn() {
  stateLED = LOW;
  digitalWrite(LED_BUILTIN, HIGH);
  response();
}

void handleLedOff() {
  stateLED = HIGH;
  digitalWrite(LED_BUILTIN, LOW);
  response();
}

const String HtmlHtml = "<html><head>"
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\" /></head>";
const String HtmlHtmlClose = "</html>";
const String HtmlTitle = "<h1>Arduino-er: ESP8266 AP WebServer exercise</h1><br/>\n";
const String HtmlLedStateLow = "<big>LED is now <b>ON</b></big><br/>\n";
const String HtmlLedStateHigh = "<big>LED is now <b>OFF</b></big><br/>\n";
const String HtmlButtons = 
    "<a href=\"LEDOn\"><button style=\"display: block; width: 100%;\">ON</button></a><br/>"
    "<a href=\"LEDOff\"><button style=\"display: block; width: 100%;\">OFF</button></a><br/>"
    "<a href=\"Device1Enabled\"><button style=\"display: block; width: 100%;\">DEVICE1 ON</button></a><br/>"
    "<a href=\"Device1Disabled\"><button style=\"display: block; width: 100%;\">DEVICE1 OFF</button></a><br/>"
     "<a href=\"Device2Enabled\"><button style=\"display: block; width: 100%;\">DEVICE2 ON</button></a><br/>"
    "<a href=\"Device2Disabled\"><button style=\"display: block; width: 100%;\">DEVICE2 OFF</button></a><br/>"
     "<a href=\"Device3Enabled\"><button style=\"display: block; width: 100%;\">DEVICE3 ON</button></a><br/>"
    "<a href=\"Device3Disabled\"><button style=\"display: block; width: 100%;\">DEVICE3 OFF</button></a><br/>"
     "<a href=\"Device4Enabled\"><button style=\"display: block; width: 100%;\">DEVICE4 ON</button></a><br/>"
    "<a href=\"Device4Disabled\"><button style=\"display: block; width: 100%;\">DEVICE4 OFF</button></a><br/>";



  void response2(){
  String htmlRes = HtmlHtml + HtmlTitle;
  htmlRes += resp;

  htmlRes += HtmlButtons;
  htmlRes += HtmlHtmlClose;

  server.send(200, "text/html", htmlRes);
}

void response(){
  String htmlRes = HtmlHtml + HtmlTitle;
  if(stateLED == LOW){
    htmlRes += HtmlLedStateLow;
  }else{
    htmlRes += HtmlLedStateHigh;
  }

  htmlRes += HtmlButtons;
  htmlRes += HtmlHtmlClose;

  server.send(200, "text/html", htmlRes);
}

void setup() {
    delay(1000);
    Serial.begin(115200);
    Serial.println();
    
    pinMode(Pin0, OUTPUT);
    pinMode(Pin1, OUTPUT);
    pinMode(Pin2, OUTPUT);
    pinMode(Pin3, OUTPUT);

    pinMode(Pin4, OUTPUT);
    
    WiFi.softAP(ssid, password);

    IPAddress apip = WiFi.softAPIP();
    Serial.print("visit: \n");
    Serial.println(apip);
    server.on("/", handleRoot);
    server.on("/LEDOn", handleLedOn);
    server.on("/LEDOff", handleLedOff);

     server.on("/Device1Enabled", handleDevice1On);
    server.on("/Device1Disabled", handleDevice1Off);

     server.on("/Device2Enabled", handleDevice2On);
    server.on("/Device2Disabled", handleDevice2Off);

     server.on("/Device3Enabled", handleDevice3On);
    server.on("/Device3Disabled", handleDevice3Off);

    server.on("/Device4Enabled", handleDevice4On);
    server.on("/Device4Disabled", handleDevice4Off);
    
    server.begin();
    Serial.println("HTTP server beginned");
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, stateLED);
}

void loop() {
    server.handleClient();
}
