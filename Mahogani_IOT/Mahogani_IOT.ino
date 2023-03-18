
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>

#ifndef STASSID
#define STASSID "fruitful"
#define STAPSK  "AwesomeGod"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;
const char* deviceName = "Mahogani";

const char* autobotSSID = "AUTOBOT";
const char* autobotPassword = "Psalm121";

const char* josephSSID = "TECNO CX Air";
const char* josephPassword = "jojo1234";

const char* host = "www.i-margin.com";

const char* extIPHelpHost = "www.myexternalip.com";    // Get external IP from https://myexternalip.com/raw

String publicIP = "";

ESP8266WebServer server(8073);

 IPAddress staticIP(192, 168, 43, 190); //ESP static ip
 IPAddress gateway(192, 168, 43, 1);   //IP Address of your WiFi Router (Gateway)
 IPAddress subnet(255, 255, 255, 0);  //Subnet mask
  IPAddress dns(8,8,8,8);

 IPAddress autobotIP(192, 168, 8, 120); //ESP static ip
 IPAddress autobotGateway(192, 168, 8, 1);   //IP Address of your WiFi Router (Gateway)
 IPAddress autobotSubnet(255, 255, 255, 0);  //Subnet mask


void handleRoot() {
  server.send(200, "text/plain", "I am working really well - relax");
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup(void) {  

  Serial.begin(115200);

  int numberOfNetworks = WiFi.scanNetworks();
 
  for(int i =0; i<numberOfNetworks; i++){
      
      Serial.print("Network name: ");
      Serial.println(WiFi.SSID(i));
      Serial.print("Signal strength: ");
      Serial.println(WiFi.RSSI(i));
      Serial.println("-----------------------");

      //if autobot is available
      if(WiFi.SSID(i).equals("AUTOBOT")){
        Serial.println("Connecting to AUTOBOT");
        WiFi.hostname(deviceName);      // DHCP Hostname (useful for finding device for static lease)
        WiFi.config(autobotIP, dns, autobotGateway, subnet); 
        WiFi.mode(WIFI_STA);
        WiFi.begin(autobotSSID, autobotPassword);

        // Wait for connection
        while (WiFi.status() != WL_CONNECTED) {
          delay(500);
          Serial.print(".");
        }

        break;
      }

//if my phone is available
      if(WiFi.SSID(i).equals("TECNO CX Air")){
        Serial.println("Connecting to Joseph");
        WiFi.hostname(deviceName);      // DHCP Hostname (useful for finding device for static lease)
        WiFi.config(staticIP, dns, gateway, subnet);
        WiFi.mode(WIFI_STA);
        WiFi.begin(josephSSID, josephPassword);

        // Wait for connection
        while (WiFi.status() != WL_CONNECTED) {
          delay(500);
          Serial.print(".");
        }
        break;
      }

      //if franca's phone is available
      if(WiFi.SSID(i).equals("fruitful")){
        Serial.println("Connecting to fruitful");
        WiFi.hostname(deviceName);      // DHCP Hostname (useful for finding device for static lease)
        WiFi.config(staticIP, dns, gateway, subnet);
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid, password);

        // Wait for connection
        while (WiFi.status() != WL_CONNECTED) {
          delay(500);
          Serial.print(".");
        }
        break;
      }
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  Serial.println("Root handled");
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });
  Serial.println("Inline set");
  server.onNotFound(handleNotFound);


if (MDNS.begin("esp8266")) {
  Serial.println("MDNS successful");
    Serial.println("MDNS responder started");
  }
  Serial.println("MDNS successful");
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  MDNS.update();
  Serial.print("Connected on>");
  Serial.println(WiFi.localIP());

  HTTPClient http;    //Declare object of class HTTPClient

 //first get public ip
    http.begin("http://api.ipify.org");  //Specify request destination
    int httpCode = http.GET();                                                                  //Send the request
     
    if (httpCode > 0) { //Check the returning code
    publicIP = http.getString();   //Get the request response payload
    Serial.println(publicIP);                     //Print the response payload
    }
    http.end();   //Close connection

 //send to server ( for record
 
    http.begin("http://i-margin.com/GeneratorStatus.php/?IP="+publicIP);  //Specify request destination
    httpCode = http.GET();                                                                  //Send the request
     
    if (httpCode > 0) { //Check the returning code
    String payload = http.getString();   //Get the request response payload
    Serial.print("response:");
    Serial.println(payload);                     //Print the response payload
    }
     
    http.end();   //Close connection


  if(WiFi.status() == WL_DISCONNECTED){
      Serial.println("Lost WiFi Connection! Restarting...");
      ESP.restart();
    }
  delay (20000);
}
